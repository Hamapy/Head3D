#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWidgets/QMainWindow>
#include <qmainwindow.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QPushButton>
#include <QtCore/QDebug>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QStackedLayout>
#include <QMessageBox.h>
#include <QtWidgets/QFileDialog>
#include <qfile.h>
#include <qfiledialog.h>
#include <qpdfwriter.h>
#include <qpainter.h>
#include <QtPrintSupport/qprinter.h>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <qsqltablemodel.h>
#include <qdatetime.h>
#include <QTextCodec>
#include <qtextdocument.h>
#include <QDesktopServices>
#include <qsqlquerymodel.h>
#include <qsqlrecord.h>
#include <qpluginloader.h>
#include <QVTKWidget.h>
#include <vtkRenderWindow.h>
#include <qpixmap.h>
#include <qscreen.h>
#include <qstringlist.h>
#include <qprogressdialog.h>
#include <QThread>
#include <qmutex.h>
#include <QObject>
#include <QtCore\qcoreapplication.h>
#include "qsettings.h"
#include "qconfig.h"
#include "qdebug.h"


#include "ui_mainWindow.h"
#include "dataselect.h"
#include "dataselect_2.h"
#include "HeadPCL.h"


//////////////////////////线程1//////////////////////////////
class WorkerThread : public QObject
{
	Q_OBJECT

public:
	WorkerThread(QObject *parent = 0);
	//~WorkerThread();//important

public slots:
	void modelInput();

signals:
	void finishedInput();
};

//////////////////////////线程2//////////////////////////////
class WorkerThread_2 : public QObject
{
	Q_OBJECT

public:
	WorkerThread_2(QObject *parent = 0);

	public slots:
	void modelInput();

signals:
	void finished();
};

//////////////////////////线程3//////////////////////////////
class WorkerThread_3 : public QObject
{
	Q_OBJECT

public:
	WorkerThread_3(QObject *parent = 0);

public slots:
	void modelInput();

signals:
	void finished_2();
};
////////////////////////////////////////////////////////////

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~MainWindow();

	dataSelect *dataselect;
	dataSelect_2 *dataselect_2;

	QProgressDialog *progressDlg;
	QThread *thread;
	WorkerThread *workerThread;
	WorkerThread_2 *workerThread_2;
	WorkerThread_3 *workerThread_3;
	//QMutex m_mutex;
	//boost::mutex cloud_mutex;

	private slots:
	void switchPage();//切换函数

	//欢迎
	void pushButton_4_pressed();
	void pushButton_pressed();
	void pushButton_2_pressed();
	void pushButton_3_pressed();

	//登录注册
	void pB_login_clicked();
	void pB_registe_clicked();
	void pB_reset_clicked();
	void cB_showPassword_clicked();

	//录入
	void selectPatient();
	void inputPatientInformation();
	void pB_dataImport_clicked();
	void uiRenew();
	void pB_cancelRePick_clicked();
	void pB_confirmRePick_clicked();
	void pB_dataRenew_clicked();
	void cB_changeKeyPoints_clicked();
	void correctKeyPoints();
	void radioButton_clicked();

	//分析
	void popNewWindow();
	void popNewWindow_2();
	void pB_dataAnalysis_clicked();
	void pB_dataAnalysis_2_clicked();
	void pB_exportPDF_clicked();
	void finishInput();
	void finishInput_2();

	//信息管理
	void tableViewerInit();
	void reset();
	void query();
	void comoBoxEditable();
	void drop();
	void addin();

public:
	void initialVtkWidget();
	void databaseInit();

	QString admin;
	QSqlTableModel *model;
	QString birthStr;
	int headTopResist = 0;

private:
	Ui::mainWindowClass ui;

	void pp_callback(const pcl::visualization::PointPickingEvent& event, void* args);
	//void pp_callback_2(const pcl::visualization::PointPickingEvent& event, void* args);

	QSqlDatabase db;


signals:
	void ready();
	void ready_2();
	void ready_3();

};

#endif


