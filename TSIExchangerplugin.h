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


#ifndef TSIExchangerPLUGIN_H
#define TSIExchangerPLUGIN_H

#include <QObject>
#include <QtWidgets>
#include <QString>
#include <Qlibrary>

#include "plugininterface.h"
#include "TSIExchanger_dialog.h"
#include "TSIExchanger.h"
#include "defines.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class TSIExchangerPlugin : public QObject, ExtensionInterface
{
    Q_OBJECT
	Q_PLUGIN_METADATA(IID "BrainStim.Plugins.Extensions" "TSIExchanger.json")
    Q_INTERFACES(ExtensionInterface)

public:
	TSIExchangerPlugin(QObject *parent = 0);
	~TSIExchangerPlugin();

	int ConfigureScriptEngine(QScriptEngine &engine);
	QString GetMinimalMainProgramVersion() {return PLUGIN_MAIN_PROGRAM_MINIMAL_VERSION;};//here we override the virtual function to set a custom value

private:

	short nExampleProperty;
	TSIExchanger *TSIExchangerObject; 
	TSIExchanger_Dialog *TSIExchangerDiagObject;

public slots:
	bool HasGUI() { return false; };
    bool ShowGUI();
	bool IsCompatible() {return PluginInterface::IsCompatible();};// ExtensionInterface PluginInterface::IsCompatible();};
	//QStringList GetAdditionalFileExtensions() {return (QStringList() << "TSIExchanger files (*.TSIExchanger);;");};
	QObject *GetScriptMetaObject(int nIndex) {if(nIndex == 0) return (QObject *)TSIExchangerObject->metaObject(); else return NULL;};

signals:
	void DoSignal();
};

#endif//TSIExchangerPLUGIN_H
