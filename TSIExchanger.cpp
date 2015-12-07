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

#include "TSIExchanger.h"
#include <QDebug>

QScriptValue TSIExchanger::ctor__extensionname(QScriptContext* context, QScriptEngine* engine)
{
	Q_UNUSED(context);
	//this function gets called first whenever a new object is constructed trough the script

	//	if (context->isCalledAsConstructor()) {
	//		// initialize the new object
	//		//context->thisObject().setProperty("bar", ...);
	//		// ...
	//		// return a non-object value to indicate that the
	//		// thisObject() should be the result of the "new Foo()" expression
	//		//return engine->undefinedValue();

	return engine->newQObject(new TSIExchanger(), QScriptEngine::ScriptOwnership);//Now call the below real Object constructor
} 

TSIExchanger::TSIExchanger(bool autoConnect, bool autoReconnect, QObject *parent) : QObject(parent)
{
	currentScriptEngine = NULL;
	tsiNetwIntFace = NULL;
	initialize(autoConnect,autoReconnect);
}

TSIExchanger::~TSIExchanger()
{
	//You don't have to manually disconnect signals and slots, the QObject destruction cleans them up automatically
	//disconnect(tsiNetwIntFace,&TSINetworkInterface::executePreStep,this,&TSIExchanger::executePreStep);
	//disconnect(tsiNetwIntFace,&TSINetworkInterface::executePostStep,this,&TSIExchanger::executePostStep);
	//disconnect(tsiNetwIntFace,&TSINetworkInterface::executePostRun,this,&TSIExchanger::executePostRun);
	//disconnect(tsiNetwIntFace,&TSINetworkInterface::disconnected,this,&TSIExchanger::disconnected);
	//disconnect(tsiNetwIntFace,&TSINetworkInterface::connected,this,&TSIExchanger::connected);
	//disconnect(tsiNetwIntFace,&TSINetworkInterface::connectionError,this,&TSIExchanger::connectionError);
	if(tsiNetwIntFace)
	{
		delete tsiNetwIntFace;
		tsiNetwIntFace = NULL;
	}
}

bool TSIExchanger::makeThisAvailableInScript(QString strObjectScriptName, QObject *engine)
{
	if (engine)
	{
		currentScriptEngine = reinterpret_cast<QScriptEngine *>(engine);
		//QObject *someObject = this;//new MyObject;
		QScriptValue objectValue = currentScriptEngine->newQObject(this);
		currentScriptEngine->globalObject().setProperty(strObjectScriptName, objectValue);
		return true;
	}
	return false;
}

bool TSIExchanger::initialize(bool autoConnect, bool autoReconnect)
{
	//if(tsiNetwIntFace == NULL)
	//{
		tsiNetwIntFace = new TSINetworkInterface(autoConnect,autoReconnect);
		
		connect(tsiNetwIntFace,&TSINetworkInterface::executeTimePoint,this,&TSIExchanger::executeTimePoint);
		connect(tsiNetwIntFace,&TSINetworkInterface::executePostRun,this,&TSIExchanger::executePostRun);
		connect(tsiNetwIntFace,&TSINetworkInterface::disconnected,this,&TSIExchanger::disconnected);
		connect(tsiNetwIntFace,&TSINetworkInterface::connected,this,&TSIExchanger::connected);
		connect(tsiNetwIntFace,&TSINetworkInterface::connectionError,this,&TSIExchanger::connectionError);
	//}
	return true;
}

bool TSIExchanger::activateAutoConnection()
{
	return tsiNetwIntFace->setAutoConnection(true);
}

bool TSIExchanger::deactivateAutoConnection()
{
	return tsiNetwIntFace->setAutoConnection(false);
}

bool TSIExchanger::activateAutoReconnect()
{
	return tsiNetwIntFace->setAutoReconnect(true);
}

bool TSIExchanger::deactivateAutoReconnect()
{
	return tsiNetwIntFace->setAutoReconnect(false);
}