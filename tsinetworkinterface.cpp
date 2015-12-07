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

#include "tsinetworkinterface.h"

TSINetworkInterface::TSINetworkInterface(bool autoConnect, bool autoReconnect)
    : QThread()
{

	VMajor = 1;
	VMinor = 6;
	VSubminor = 0;

	tcpSocket = new QTcpSocket();
	tcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
	tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

	rTcpSocket = new QTcpSocket();
	rTcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
	rTcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);

	nDefaultPort = (quint16)55555;
	blockSize = 0;
	terminateReconnect = false;
	terminateReciever = false;
	manualDisconnect = false;
	udpAutoConnect = autoConnect;
	reConnect = autoReconnect;
	udpSocket = NULL;

	connect(tcpSocket,SIGNAL(connected()),this,SLOT(connectionEstablished()));
	connect(rTcpSocket,SIGNAL(disconnected()),this,SLOT(connectionLost()));
	connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(writeError(QAbstractSocket::SocketError)));

	querryQueue = new QQueue<QString>;
		
	if(udpAutoConnect)
		setAutoConnection(true);
}

TSINetworkInterface::~TSINetworkInterface()
{
	terminateReconnect = true;
	terminateReciever = true;

	if(tcpSocket)
	{
		delete tcpSocket;
		tcpSocket = NULL;
	}
	if(rTcpSocket)
	{
		delete rTcpSocket;
		rTcpSocket = NULL;
	}
	if(querryQueue)
	{
		delete querryQueue;
		querryQueue = NULL;
	}
	if(udpSocket)
	{
		delete udpSocket;
		udpSocket = NULL;
	}
}


bool TSINetworkInterface::isConnected()
{
	return tcpSocket->state()==QAbstractSocket::ConnectedState;
}

void TSINetworkInterface::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
		QHostAddress *TSIServer = new QHostAddress();
		quint16 *TSIPort = new quint16();
		udpSocket->readDatagram(datagram.data(),datagram.size(),TSIServer,TSIPort);

		quint16 port = tr(datagram.data()).remove("Turbo-BrainVoyager Broadcast: ").toInt();


		if(tr(datagram.data()).contains(tr("Turbo-BrainVoyager Broadcast")))
		{
			if(!connectToServer(TSIServer->toString().toLatin1().data(),port))
			{
				udpSocket->close();
				writeError(tcpSocket->error());
			}
			else
			{
				reConnect = false;
			}
			return;
		}
    }
}

void TSINetworkInterface::connectionEstablished()
{
	if(udpSocket)
		udpSocket->close();
	emit connected(ipAddressString,iPort);
}

void TSINetworkInterface::connectionLost()
{
	emit disconnected();
	if(manualDisconnect)
		return;
	if(reConnect)
		tryToReconnect();
	else if(udpAutoConnect)
		setAutoConnection(true);
}

void TSINetworkInterface::tryToReconnect()
{
	if (udpSocket)
		udpSocket->close();
	if (tcpSocket)
	{
		tcpSocket->readAll();
		tcpSocket->close();
	}
	if (rTcpSocket)
	{
		rTcpSocket->readAll();
		rTcpSocket->close();
	}

	disconnect(rTcpSocket, SIGNAL(readyRead()), this, SLOT(readExecuteStep()));

	QTime *timer = new QTime();
	emit connecting();
	while (!terminateReconnect && !connectToServer(ipAddressString.toUtf8().data(),iPort))
	{
		timer->restart();
		while(timer->elapsed()<2000)
		{
			QCoreApplication::processEvents();
			msleep(1);
		}
	}
}
bool TSINetworkInterface::connectToServer(char *ipAddress,quint16 port)
{
	manualDisconnect = false;
	ipAddressString = ipAddress;
	iPort = port;
	tcpSocket->connectToHost(tr(ipAddress),port);
	if(!tcpSocket->waitForConnected(2000))
		return false;

	sendStreamDefinition("Request Socket",tcpSocket);
	if(!checkConnection("Request Socket",tcpSocket))
	{
		tcpSocket->close();
		return false;
	}

	msleep(100);
	rTcpSocket->connectToHost(tr(ipAddress),port);
	if(!rTcpSocket->waitForConnected(2000))
	{
		tcpSocket->close();
		return false;
	}
	sendStreamDefinition("Execute Socket",rTcpSocket);
	if(!checkConnection("Execute Socket",rTcpSocket))
	{
		rTcpSocket->close();
		tcpSocket->close();
		return false;
	}
	connect(rTcpSocket,SIGNAL(readyRead()),this,SLOT(readExecuteStep()));

	return true;
}


bool TSINetworkInterface::sendStreamDefinition(char *definition,QTcpSocket *dSocket)
{
	QByteArray definit;
	QDataStream out(&definit, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_4_8);
	out.setFloatingPointPrecision(QDataStream::SinglePrecision);
	out << (qint64)0;
	out << definition;	
	out.device()->seek(0);
	out << (qint64)(definit.size() - sizeof(qint64));

	if(dSocket->write(definit) == 0)
	{
		if(dSocket->state() != QAbstractSocket::ConnectedState)
			return false;
	}
	
	if(!dSocket->waitForBytesWritten())
	{
		return false;
	}

	
	return true;

}

bool TSINetworkInterface::checkConnection(QString socketName, QTcpSocket *dSocket)
{
	while(!terminateReciever)
	{
		QDataStream in(dSocket);
		in.setVersion(QDataStream::Qt_4_8);
		in.setFloatingPointPrecision(QDataStream::SinglePrecision);
		dSocket->waitForReadyRead(1000);
		//get size of data to receive
		if (blockSize == 0) {
			if (dSocket->bytesAvailable() < (int)sizeof(qint64))
			{
				return false;
			}
			in >> blockSize;
		}
		//ensure that enough data is on the stream
		if ((qint64)dSocket->bytesAvailable() < blockSize)
		{
			return false;
		}

		blockSize = 0;

		char *rcvSocketName;
		in >> rcvSocketName;
		int major, minor, subminor;
		in >> major;
		in >> minor;
		in >> subminor;

		if(!rcvSocketName)
		{
			while (!dSocket->atEnd()) 
			{
				QByteArray data = dSocket->read(100);
			}
		}


		if(tr(rcvSocketName) == socketName && major == VMajor && minor >= VMinor)
			return true;
		else
			return false;

	}
	return false;
}

bool TSINetworkInterface::disconnectFromServer()
{	
	manualDisconnect = true;
	udpSocket->close();
	tcpSocket->close();
	rTcpSocket->close();
	return true;
}

bool TSINetworkInterface::setAutoConnection(bool setting)
{
	if(setting)
	{
		if(udpSocket == NULL)
			udpSocket = new QUdpSocket();

		bool bBindResult = udpSocket->bind(nDefaultPort, QUdpSocket::ShareAddress);
		bool bConnectResult = connect(udpSocket, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));
		return bConnectResult && bBindResult;
	}
	else
	{
		if(udpSocket)
		{
			disconnect(udpSocket, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));
			udpSocket->close();
			delete udpSocket;
			udpSocket = NULL;
		}
		return false;
	}
}

void TSINetworkInterface::readExecuteStep()
{
	while(!terminateReciever)
	{
		QDataStream in(rTcpSocket);
		in.setVersion(QDataStream::Qt_4_8);
		in.setFloatingPointPrecision(QDataStream::SinglePrecision);
		//get size of data to receive
		if (blockSize == 0) {
			if (rTcpSocket->bytesAvailable() < (int)sizeof(qint64))
			{
				return;
			}
			in >> blockSize;
		}
		//ensure that enough data is on the stream
		if ((qint64)rTcpSocket->bytesAvailable() < blockSize)
		{
			return;
		}

		blockSize = 0;
		char *rcvChar;
		int timepoint;
		in >> rcvChar;

		if(!rcvChar)
		{
			while (!rTcpSocket->atEnd()) 
			{
				QByteArray data = rTcpSocket->read(100);
			}
		}
		else
		if(strcmp(rcvChar , "TimePointStepCalled") == 0)
		{
			in >> timepoint;
			emit executeTimePoint(timepoint);	
		}
		else
		if(strcmp(rcvChar , "PostRunCalled") == 0)
		{
			in >> timepoint;
			emit executePostRun(timepoint);
		}
		else
		while (!rTcpSocket->atEnd()) 
		{
			  QByteArray data = tcpSocket->read(100);
		}
	}
}

//Basic TSI Project Functions
int TSINetworkInterface::tGetCurrentTimePoint()
{
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetCurrentTimePoint"));
	doRequest.sendData("tGetCurrentTimePoint");
	QString status = doRequest.getReply();
	
	if(!status.isEmpty())
	{
		return 1337;
	}
	int CurrentTimePoint;
	doRequest.getDataOfByteArray(CurrentTimePoint);
	return CurrentTimePoint;
}
//int	TSINetworkInterface::tGetExpectedNrOfTimePoints()
//{
//	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetExpectedNrOfTimePoints"));
//	doRequest.sendData("tGetExpectedNrOfTimePoints");
//	QString status = doRequest.getReply();
//
//	if(!status.isEmpty())
//	{
//		return 0;
//	}
//	int ExpectedNrOfTimePoints;
//	doRequest.getDataOfByteArray(ExpectedNrOfTimePoints);
//	return ExpectedNrOfTimePoints;
//}

QString TSINetworkInterface::tGetProjectName()
{
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetProjectName"));
	doRequest.sendData("tGetProjectName");
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return "";
	}
	char *ProjectName;
	doRequest.getDataOfByteArray(ProjectName);
	return QString(ProjectName);
}
QString TSINetworkInterface::tGetValuesFeedbackFolder()
{
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetValuesFeedbackFolder"));
	doRequest.sendData("tGetValuesFeedbackFolder");
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return "";
	}
	char *WatchFolder;
	doRequest.getDataOfByteArray(WatchFolder);
	return QString(WatchFolder);
}
QString TSINetworkInterface::tGetImagesFeedbackFolder()
{
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetImagesFeedbackFolder"));
	doRequest.sendData("tGetImagesFeedbackFolder");
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return "";
	}
	char *TargetFolder;
	doRequest.getDataOfByteArray(TargetFolder);
	return QString(TargetFolder);
}

int TSINetworkInterface::tGetNrOfSelectedChannels()
{
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetNrOfSelectedChannels"));
	doRequest.sendData("tGetNrOfSelectedChannels");
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return 0;
	}
	int NrOfSelectedChannels;
	doRequest.getDataOfByteArray(NrOfSelectedChannels);
	return NrOfSelectedChannels;
}
QList<int> TSINetworkInterface::tGetSelectedChannels()
{
	int NrOfSelectedChannels = tGetNrOfSelectedChannels();

	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetSelectedChannels"));
	doRequest.sendData("tGetSelectedChannels");
	QString status = doRequest.getReply();

	QList<int> SelectedChannels;

	if(!status.isEmpty())
	{
		SelectedChannels.append(0);
		return SelectedChannels;
	}
	int temp = 0;
	for(int i=0;i<NrOfSelectedChannels;i++)
	{
		doRequest.getDataOfByteArray(temp);
		SelectedChannels.append(temp);
	}
	return SelectedChannels;
}

int TSINetworkInterface::tGetNrOfChannels()
{
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetNrOfChannels"));
	doRequest.sendData("tGetNrOfChannels");
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return 0;
	}
	int NrOfChannels;
	doRequest.getDataOfByteArray(NrOfChannels);
	return NrOfChannels;
}

float TSINetworkInterface::tGetRawDataScaleFactor()
{
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetRawDataScaleFactor"));
	doRequest.sendData("tGetRawDataScaleFactor");
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return 0;
	}
	float RawDataScaleFactor;
	doRequest.getDataOfByteArray(RawDataScaleFactor);
	return RawDataScaleFactor;
}

float TSINetworkInterface::tGetRawDataWL1(int channel, int timePoint)
{	
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetRawDataWL1"));
	doRequest.sendData("tGetRawDataWL1",channel, timePoint);
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return 0;
	}
	float RawDataWL1;
	doRequest.getDataOfByteArray(RawDataWL1);
	return RawDataWL1;
}

float TSINetworkInterface::tGetRawDataWL2(int channel, int timePoint)
{	
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetRawDataWL2"));
	doRequest.sendData("tGetRawDataWL2",channel, timePoint);
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return 0;
	}
	float RawDataWL2;
	doRequest.getDataOfByteArray(RawDataWL2);
	return RawDataWL2;
}

int TSINetworkInterface::tIsDataOxyDeoxyConverted()
{
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tIsDataOxyDeoxyConverted"));
	doRequest.sendData("tIsDataOxyDeoxyConverted");
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return 0;
	}
	int IsDataOxyDeoxyConverted;
	doRequest.getDataOfByteArray(IsDataOxyDeoxyConverted);
	return IsDataOxyDeoxyConverted;
}

float TSINetworkInterface::tGetOxyDataScaleFactor()
{
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetOxyDataScaleFactor"));
	doRequest.sendData("tGetOxyDataScaleFactor");
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return 0;
	}
	float xyDataScaleFactor;
	doRequest.getDataOfByteArray(xyDataScaleFactor);
	return xyDataScaleFactor;
}

float TSINetworkInterface::tGetDataOxy(int channel, int timePoint)
{	
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetDataOxy"));
	doRequest.sendData("tGetDataOxy",channel, timePoint);
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return 0;
	}
	float DataOxy;
	doRequest.getDataOfByteArray(DataOxy);
	return DataOxy;
}

float TSINetworkInterface::tGetDataDeoxy(int channel, int timePoint)
{	
	TSI_Server_Request doRequest(tcpSocket,querryQueue,tr("tGetDataDeoxy"));
	doRequest.sendData("tGetDataDeoxy",channel, timePoint);
	QString status = doRequest.getReply();

	if(!status.isEmpty())
	{
		return 0;
	}
	double DataDeoxy;
	doRequest.getDataOfByteArray(DataDeoxy);
	return DataDeoxy;
}


void TSINetworkInterface::writeError(QAbstractSocket::SocketError Error)
 {
     switch (Error) {
	 case QAbstractSocket::RemoteHostClosedError:
		 emit connectionError(tr("The remote host closed the connection.\n Please restart the remote host."));
		 break;
     case QAbstractSocket::HostNotFoundError:
         emit connectionError(tr("The host was not found. Please check the host name and port settings."));
         break;
     case QAbstractSocket::ConnectionRefusedError:
         emit connectionError(tr("The connection was refused by the peer. Make sure the server is running, and check that the host name and port settings are correct."));
         break;
     default:
         emit connectionError(tr("The following error occurred: %1. \nCheck the firewall settings!").arg(tcpSocket->errorString()));
     }

 }

bool TSINetworkInterface::setAutoReconnect(bool setting)
{
	reConnect = setting;
	return true;
}