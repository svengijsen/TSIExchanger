//TSIExchanger
//Copyright (C) 2015  Michael Luehrs
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


#include "TSIExchangerplugin.h"
#include "TSIExchanger.h"
#include "defines.h"

Q_DECLARE_METATYPE(TSIExchanger*)
Q_DECLARE_METATYPE(TSIExchanger)

TSIExchangerPlugin::TSIExchangerPlugin(QObject *parent)
{
	TSIExchangerObject = new TSIExchanger(parent);
	TSIExchangerDiagObject = new TSIExchanger_Dialog();	
	strPluginInformation = QString(PLUGIN_INTERNAL_NAME) + " Plugin" + "(v" + PLUGIN_FILE_VERSION_STRING + ")";// +" by " + PLUGIN_AUTHOR_NAME;
	strPluginInternalName = PLUGIN_INTERNAL_NAME;
}

TSIExchangerPlugin::~TSIExchangerPlugin()
{
	delete TSIExchangerDiagObject;
	delete TSIExchangerObject;
}

int TSIExchangerPlugin::ConfigureScriptEngine(QScriptEngine &engine)
{
	QScriptValue TSIExchangerProto = engine.newQObject(TSIExchangerObject);
	engine.setDefaultPrototype(qMetaTypeId<TSIExchanger*>(), TSIExchangerProto);
	QScriptValue TSIExchangerCtor = engine.newFunction(TSIExchanger::ctor_TSIExchanger, TSIExchangerProto);
	engine.globalObject().setProperty(PLUGIN_SCRIPTOBJECT_NAME, TSIExchangerCtor);
	int nMetaType = qRegisterMetaType<TSIExchanger>(PLUGIN_SCRIPTOBJECT_NAME);
	//FinalizeScriptObjectScriptContext(engine, TSIExchangerObject);
	return nMetaType;
}

bool TSIExchangerPlugin::ShowGUI()
{
	int returnVal;
	TSIExchangerDiagObject->setWindowTitle(strPluginInformation);
	returnVal = TSIExchangerDiagObject->exec();

	switch (returnVal) {
	case QMessageBox::Save:
	   // Save was clicked
	   break;
	case QMessageBox::Discard:
		// Don't Save was clicked
	    break;
	case QMessageBox::Cancel:
	    // Cancel was clicked
	    break;
	default:
	    // should never be reached
	    break;
	}		
	return true;
}

