//TSIExchanger
//Copyright (C) 2015  Sven Gijsen
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


//This file implements the UI functionality

#ifndef TSIExchanger_DIALOG_H
#define TSIExchanger_DIALOG_H

#include <QDialog>
#include "ui_TSIExchanger_dialog.h"
#include <QtWidgets>
#include <Windows.h>
#include <QDialog>

class TSIExchanger_Dialog : public QDialog, private Ui::TSIExchanger_DialogClass
{
	Q_OBJECT

public:
	TSIExchanger_Dialog(QWidget *parent = 0);
	~TSIExchanger_Dialog();

private:
	Ui::TSIExchanger_DialogClass ui;
	void cleanUp();
	
private slots:
	void on_cancelButton_clicked();
	void exampleButton_Pressed();
	void on_okButton_clicked();
};

#endif // TSIExchanger_DIALOG_H
