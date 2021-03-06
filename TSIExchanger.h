//Copyright (C) 2015  Michael Luehrs, Brain Innovation B.V.
//
//This file is part of BrainStim.
//BrainStim is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

//This file defines the script binding interface, all below function are scriptable except for the destructor

#ifndef TSIEXCHANGER_H
#define TSIEXCHANGER_H
#include <QObject>
#include <QString>
#include <QtScript>
#include <QScriptable>
#include "mainappinfo.h"
#include "tsinetworkinterface.h"

template <typename T,unsigned S>
inline unsigned arraysize(const T (&v)[S]) { return S; }

//!  The TSIExchanger class. 
/*!
  The TSIExchanger can be used to communicate with the Turbo-Satori software from Brain Innovation (see http://www.brainvoyager.com/products/turbosatori.html).
*/
class TSIExchanger : public QObject, protected QScriptable
{
	Q_OBJECT
	//Q_CLASSINFO("ScriptAPIClassName", "TSIExchanger");//Can't use defines here!, moc doesn't handle defines, not needed here

signals:
	//!  executePreStep signal. 
	/*!
	  This signal will be called whenever new data is available in the TSI.
	*/
	void executeTimePoint(int timePoint);
	//!  executePostRun signal. 
	/*!
	  This signal will be called when the TSI finished processing the experiment run.
	*/
	void executePostRun(int timePoint);
	//!  disconnected signal.
	/*!
	  This signal will be called when the connection is interrupted.
	*/
	void disconnected();
	//!  connected signal.
	/*!
	  This signal will be called when a connection to the TSI was established.
	*/
	void connected();
	//!  connectionError signal. 
	/*!
	  This signal will be called when a connection error occurs. The error is described in the received QString.
	*/
	void connectionError(QString error);

public:
	TSIExchanger(bool autoConnect = false, bool autoReconnect = true, QObject *parent = 0);
	~TSIExchanger();
	TSIExchanger(const TSIExchanger& other ){Q_UNUSED(other);}//TODO fill in copy constructor, should be used for the Q_DECLARE_METATYPE macro

	static QScriptValue ctor_TSIExchanger(QScriptContext* context, QScriptEngine* engine);

public slots:
	bool makeThisAvailableInScript(QString strObjectScriptName = "", QObject *engine = NULL);//To make the objects (e.g. defined in a *.exml file) available in the script

//Basic Project Functions
/** @name Basic Project Functions
	@anchor BasicProjectFunctions
*/
//@{
	//!  tGetCurrentTimePoint slot. 
	/*!
	  Provides the number of the currently processed step during real-time processing as an integer. Note that this function is 1-based, i.e. when the first step is processed the function returns "1" not "0"; this is important when the return value is used to access time-related information; in this case subtract "1" from the returned value.
	*/
	int tGetCurrentTimePoint();
	//!  tGetNrOfChannels slot. 
	/*!
	  Provides the number of time points as an integer. The name contains the term "expected" since a real-time run might be interrupted by the user, i.e. this is the intended number of volumes as specified in the TSI settings file.
	*/
	int tGetNrOfChannels();
	//!  tGetValuesFeedbackFolder slot. 
	/*!
	  Provides the dimensions of the functional volumes; "dim_x" and "dim_y" are the dimensions of the slices constituting the volume and "dim_z" corresponds to the number of slices.
	*/
	QString tGetValuesFeedbackFolder();
	//!  tGetImagesFeedbackFolder slot. 
	/*!
	  Provides the name of the project as specified in the TSI file as a C string; note that the received data must point to a pre-allocated array that is large enough (a buffer of 100 bytes should be sufficient). The returned name can, for example, be used as part of names identifying exported data (see example "Export Volume Data" client).
	*/
	QString tGetImagesFeedbackFolder();
	//!  tGetNrOfSelectedChannels slot. 
	/*!
	  Provides the path of the "watch folder" as specified in the TSI file as a C string; Note that the received data must point to a pre-allocated array that is large enough for the returned path (a buffer of 513 bytes is recommended).
	*/
	int tGetNrOfSelectedChannels();
//@}

//Selected Channels Info Functions
/** @name Selected Channels Info Functions
	@anchor SelectedChannelsInfoFunctions
*/
//@{
	//!  tGetSelectedChannels slot. 
	/*!
	  Provides the path of the "target folder" as specified in the TSI file as a C string; note that the received data must point to a pre-allocated array that is large enough for the returned path (a buffer of 513 bytes is recommended). The target folder can be used to export data for custom processing (see example "Export Volume Data" client).
	*/
	QList<int> tGetSelectedChannels();
	//!  tGetRawDataScaleFactor slot. 
	/*!
	  Provides the path of the "feedback folder" as specified in the TSI file as a C string; note that the provided data must point to a pre-allocated array that is large enough for the received path (a buffer of 513 bytes is recommended). The feedback folder can be used to store the result of custom calculations, e.g. providing custom input for the "Presenter" software tool.
	*/
	float tGetRawDataScaleFactor();
	//!  tGetRawDataWL1 slot. 
	/*!
	  Provides the index of the currently "active" condition of the protocol (0-based), i.e. the condition that has a defined interval enclosing the current time point.
	*/
	float tGetRawDataWL1(int channel, int timePoint);
	//!  tGetRawDataWL2 slot. 
	/*!
	  Provides the number of predictors of the design matrix. Note that this query returns the "full" number of intended predictors while the "tGetCurrentNrOfPredictors" returns the number of predictors currently in use.
	*/
	float tGetRawDataWL2(int channel, int timePoint);
	//!  tIsDataOxyDeoxyConverted slot. 
	/*!
	  Provides the currently effective number of predictors. Note that this query may return a smaller number than the "tGetFullNrOfPredictors" query since the internal GLM calculations use a restricted set of predictors in case that for one or more predictors not enough non-zero data points are available. Roughly speaking, the number of current predictors increases each time when a new condition is encountered during real-time processing.
	*/
	int tIsDataOxyDeoxyConverted();
	//!  tGetOxyDataScaleFactor slot. 
	/*!
	  Provides the full number of confound predictors. To get the full/effective number of predictors-of-interest, subtract the returned value from the "tGetFullNrOfPredictors" or "tGetCurrentNrOfPredictors" function, respectively.
	*/
	float tGetOxyDataScaleFactor();
	//!  tGetDataOxy slot. 
	/*!
	  Provides the value of a predictor at a given time point of the current design matrix. Note that the design matrix always contains the "full" set of predictors, a reduced set of predictors is only used internally (predictors that are not used internally are those containing only "0.0" entries up to the current time point). Note that the "timepoint" parameter must be smaller than the value returned by the "tGetCurrentTimePoint" query. For details, see the provided example clients.
	*/
	float tGetDataOxy(int channel, int timePoint);
	//!  tGetDataDeoxy slot. 
	/*!
	  Provides the number of (automatically or manually) specified contrasts in the TSI settings file. This value is important for accessing t maps, see the "tGetMapValueOfVoxel" and "tGetContrastMaps" queries.
	*/
	float tGetDataDeoxy(int channel, int timePoint);
//@}

//Connection Functions
/** @name Connection Functions
@anchor ConnectionFunctions
*/
//@{
	//Connection functions
	//!  connectToServer slot. 
	/*!
	  Connect to the server using ip and port.
	*/
	bool connectToServer(QString sIPAddress, int port);
	//!  disconnectFromServer slot. 
	/*!
	  Disconnect from the server.
	*/
	bool disconnectFromServer();
	//!  activateAutoConnection slot. 
	/*!
	  Activates the automatic connect functionality to connect to a TSI server if available and accessible.
	*/
	bool activateAutoConnection();
	//!  deactivateAutoConnection slot. 
	/*!
	  Deactivates the automatic connect functionality.
	*/
	bool deactivateAutoConnection();
	//!  activateAutoConnection slot. 
	/*!
	Activates the automatic Reconnect functionality to connect to a TBV server if available and accessible.
	*/
	bool activateAutoReconnect();
	//!  deactivateAutoReconnect slot. 
	/*!
	Deactivates the automatic Reconnect functionality.
	*/
	bool deactivateAutoReconnect();
//@}

private:
	bool initialize(bool autoConnect, bool autoReconnect);

	QScriptEngine* currentScriptEngine;
	short m_ExampleProperty;
	TSINetworkInterface *tsiNetwIntFace;
};

#endif // TSIEXCHANGER_H
