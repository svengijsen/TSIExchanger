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

#include <QtNetwork>
#include <iostream>
#include <QQueue>
#include <QThread>
#include <QDataStream>

class TSI_Server_Request
{

public:

	TSI_Server_Request(QTcpSocket *tcpSocket, QQueue<QString> *querryQueue, QString querry);
	~TSI_Server_Request();

	bool sendData(char *Querry,int value0 = -1, int value1 = -1, int value2 = -1, int value3 = -1);
	QString getReply();
	template< typename Turb> void getDataOfByteArray(Turb &value);
	template< class T> void getVectorDataOfByteArray(QVector<T> *data,int size);

private:
	QByteArray rcvData;
	QDataStream rcvStream;
	int nrOfValues;
	QTcpSocket *eTcpSocket;
	bool sendQByteArray(QByteArray dataToSend);
	qint64 blockSize;
	QQueue<QString> *querryQueueinternal;
	QString iQuerry;

};

template< typename Turb> void TSI_Server_Request::getDataOfByteArray(Turb &value)
{
	rcvStream.setVersion(QDataStream::Qt_4_8);
	rcvStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	rcvStream.setByteOrder(QDataStream::BigEndian);
	rcvStream >> value;
}

template< class T> void TSI_Server_Request::getVectorDataOfByteArray(QVector<T> *data,int size)
{
	rcvStream.setVersion(QDataStream::Qt_4_8);
	rcvStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
	T temp;
	for (int i=0;i<size;i++)
	{
		rcvStream >> temp;
		data->replace(i,temp);
	}
}
