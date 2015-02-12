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

#include "TSIExchanger_dialog.h"

TSIExchanger_Dialog::TSIExchanger_Dialog(QWidget *parent)	: QDialog(parent)
{
	//Gets constructed only once during the load of the plugin
	ui.setupUi(this);

	QFile *fDoc = new QFile(":/dataTSI/html/NetworkPlugin_guide_v100.htm");
	QString html;
	if (fDoc->exists())
	{
		if (fDoc->open(QIODevice::ReadOnly))
		{
			html = fDoc->readAll();
			fDoc->close();
		}
	}
	doc = new QTextDocument();
	doc->setHtml(html);
	doc->addResource(QTextDocument::ImageResource, QUrl("NetworkPlugin_guide_files/image001.png"), QPixmap(":/dataTSI/html/NetworkPlugin_guide_files/image001.png"));
	doc->addResource(QTextDocument::ImageResource, QUrl("NetworkPlugin_guide_files/image002.png"), QPixmap(":/dataTSI/html/NetworkPlugin_guide_files/image002.png"));
	doc->addResource(QTextDocument::ImageResource, QUrl("NetworkPlugin_guide_files/image003.png"), QPixmap(":/dataTSI/html/NetworkPlugin_guide_files/image003.png"));
	doc->addResource(QTextDocument::ImageResource, QUrl("NetworkPlugin_guide_files/image004.png"), QPixmap(":/dataTSI/html/NetworkPlugin_guide_files/image004.png"));
	doc->addResource(QTextDocument::ImageResource, QUrl("NetworkPlugin_guide_files/image005.png"), QPixmap(":/dataTSI/html/NetworkPlugin_guide_files/image005.png"));
	doc->addResource(QTextDocument::ImageResource, QUrl("NetworkPlugin_guide_files/image006.png"), QPixmap(":/dataTSI/html/NetworkPlugin_guide_files/image006.png"));
	doc->addResource(QTextDocument::ImageResource, QUrl("NetworkPlugin_guide_files/image007.png"), QPixmap(":/dataTSI/html/NetworkPlugin_guide_files/image007.png"));
	doc->addResource(QTextDocument::ImageResource, QUrl("NetworkPlugin_guide_files/image008.png"), QPixmap(":/dataTSI/html/NetworkPlugin_guide_files/image008.png"));
	ui.textBrowser->setDocument(doc);
}

TSIExchanger_Dialog::~TSIExchanger_Dialog()
{

}

void TSIExchanger_Dialog::on_okButton_clicked()
{
	cleanUp();
	accept();
}

void TSIExchanger_Dialog::on_cancelButton_clicked()
{
	cleanUp();
	reject();
}

void TSIExchanger_Dialog::cleanUp()
{
	return;
}

void TSIExchanger_Dialog::exampleButton_Pressed()
{
	QMessageBox msgBox;
	msgBox.setText("Example button pressed.");
	msgBox.exec();
}
