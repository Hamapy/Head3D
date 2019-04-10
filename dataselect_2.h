#pragma once  
#ifndef DATASELECT_2_H
#define DATASELECT_2_H  //防止重复包含的宏定义

#include "ui_dataselect.h"
#include <qmainwindow.h>
#include <qsqltablemodel.h>
#include <QSqlQuery>
#include <qcompleter.h>
#include <QMessageBox.h>
#include <QSqlDatabase>
#include <qtimer.h>
#include <QtCore\qcoreapplication.h>
#include "qsettings.h"
//#include "mainWindow.h" //这很关键 防止类间交叉包含

class dataSelect_2 : public QMainWindow
{
	Q_OBJECT

public:
	dataSelect_2(QWidget *parent = 0);
	~dataSelect_2();

	int flag;

	public slots:
	void dataSelectInit();
	void query();
	void reset();
	void confirm();
	void comoBoxEditable();

public:
	Ui::dataSelect ui;
	QSqlTableModel *model;
	int curIndex;

private:
	QSqlDatabase db;

signals:
	void selectIndex();
	void selectIndex_2();
	void selectPatient();
};

#endif