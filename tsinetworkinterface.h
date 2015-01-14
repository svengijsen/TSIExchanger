//Copyright (C) 2015  Michael Luehrs, Brain Innovation B.V.

#include <QDataStream>
#include <QtNetwork>
#include "tsi_server_request.h"

class TSINetworkInterface : public QThread
{
	Q_OBJECT

public:
	TSINetworkInterface(bool autoConnect = false, bool autoReconnect = true);
	~TSINetworkInterface();

//	int tGetExpectedNrOfTimePoints();
	QString tGetProjectName();

	// TSI info
	int tGetCurrentTimePoint();

	int tGetNrOfChannels();

	QString tGetValuesFeedbackFolder();
	QString tGetImagesFeedbackFolder();

	// Selected channel data
	 int tGetNrOfSelectedChannels();

	 QList<int> tGetSelectedChannels();

	// Get raw data values
	 float tGetRawDataScaleFactor();

	 float tGetRawDataWL1(int channel, int timePoint);
	 float tGetRawDataWL2(int channel, int timePoint);

	 // Get preprocessed oxy/deoxy data (or preprocessed raw data if not oxy converted)
	 int tIsDataOxyDeoxyConverted();

	 float tGetOxyDataScaleFactor();

	 float tGetDataOxy(int channel, int timePoint);

	 float tGetDataDeoxy(int channel, int timePoint);

	bool connectToServer(char *ipAddress,quint16 port);
	bool disconnectFromServer();

	bool isConnected();

	bool setAutoConnection(bool setting);

private:
	quint16 nDefaultPort;
	QUdpSocket *udpSocket;
	QTcpSocket *tcpSocket;
	QTcpSocket *rTcpSocket;
	qint64 blockSize;
	QQueue<QString> *querryQueue;
	QVector<short int> TimeCourseData;
	QVector<short int> RawTimeCourseData;
	QVector<float> AllMeansOfROI;
	QVector<double> outputVector;
	QVector<double> BetaMaps;
	QVector<float> ContrastMaps;
	QVector<int> CoordsOfVoxelsOfROI;
	QVector<float> ClassifierOutput;
	bool sendStreamDefinition(char *definition,QTcpSocket *dSocket);
	bool checkConnection(QString socketName, QTcpSocket *dSocket);
	bool udpAutoConnect, reConnect, terminateReconnect, terminateReciever, manualDisconnect;
	QString ipAddressString;
	quint16 iPort;
	void tryToReconnect();
	int VMajor,VMinor,VSubminor;

signals:
	void executeTimePoint(int timepoint);
	void executePostRun(int timepoint);
	void disconnected();
	void connected(QString iPAddress, quint16 port);
	void connectionError(QString error);
	void connecting();

private slots:
	void readExecuteStep();
	void connectionEstablished();
	void connectionLost();
	void writeError(QAbstractSocket::SocketError Error);
	void processPendingDatagrams();
};


