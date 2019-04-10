#pragma once
#pragma warning(disable:4819)
#include "ui_mainWindow.h"
#include "mainWindow.h"

Head John;

//////////////////////////�߳�1///////////////////////////////////
WorkerThread::WorkerThread(QObject *parent) :
QObject(parent)
{

}

void WorkerThread::modelInput()
{
	QMutex m_mutex;	
	m_mutex.lock();
	John.cUpRight.reset(new pcl::PointCloud<pcl::PointXYZ>);
	John.keyPtr.reset(new pcl::PointCloud<pcl::PointXYZ>);
	//qDebug() << "receive thread ID:" << QThread::currentThreadId() << '\n';
	
	John.cUpRight = John.alignModel(John.filename, John.cUpRight);
	John.keyPtr = John.getKeyPoints(John.keyPtr, John.cUpRight);
	John.mesh = John.restruction(John.cUpRight);
	
	emit finishedInput();
	m_mutex.unlock();
}

//////////////////////////�߳�2///////////////////////////////////
WorkerThread_2::WorkerThread_2(QObject *parent) :
QObject(parent)
{

}

void WorkerThread_2::modelInput()
{
	QMutex m_mutex;	
	m_mutex.lock();
	//qDebug() << "receive thread ID:" << QThread::currentThreadId() << '\n';
	John.cloud.reset(new pcl::PointCloud<pcl::PointXYZ>);
	//John.viewer_2.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	pcl::io::loadPCDFile(John.filename, *(John.cloud));
	John.mesh = John.restruction(John.cloud);
	John.cloud.reset(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::io::loadPCDFile(John.filename_keyPoints, *(John.cloud));

	//qDebug() << "input finished" << QThread::currentThreadId() << '\n';
	emit finished();
	m_mutex.unlock();
}
//////////////////////////////////////////////////////////////////

//////////////////////////�߳�3///////////////////////////////////
WorkerThread_3::WorkerThread_3(QObject *parent) :
QObject(parent)
{

}

void WorkerThread_3::modelInput()
{
	QMutex m_mutex;
	m_mutex.lock();
	//qDebug() << "receive thread ID:" << QThread::currentThreadId() << '\n';
	John.cloud.reset(new pcl::PointCloud<pcl::PointXYZ>);
	//John.viewer_3.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	pcl::io::loadPCDFile(John.filename, *(John.cloud));
	John.mesh = John.restruction(John.cloud);
	John.cloud.reset(new pcl::PointCloud<pcl::PointXYZ>);
	pcl::io::loadPCDFile(John.filename_keyPoints, *(John.cloud));

	emit finished_2();
	m_mutex.unlock();
}
/////////////////////////////////////////////////////////////////



/////////////////////////UI����//////////////////////////////////
MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);//�������
	this->setFixedSize(this->width(), this->height());
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
	initialVtkWidget();
	
	databaseInit();

	this->setWindowTitle(QStringLiteral("��άͷ­���������ϵͳ"));
	this->ui.stackedWidget->addWidget(ui.page);
	this->ui.stackedWidget->addWidget(ui.page_2);
	this->ui.stackedWidget->addWidget(ui.page_3);
	this->ui.stackedWidget->addWidget(ui.page_4);
	this->ui.stackedWidget->addWidget(ui.page_5);
	this->ui.stackedWidget->addWidget(ui.page_6);

	////��ӭҳ��ռ�
	connect(this->ui.pushButton, SIGNAL(pressed()), this, SLOT(pushButton_pressed()));
	connect(this->ui.pushButton_2, SIGNAL(pressed()), this, SLOT(pushButton_2_pressed()));
	connect(this->ui.pushButton_3, SIGNAL(pressed()), this, SLOT(pushButton_3_pressed()));
	connect(this->ui.pushButton_4, SIGNAL(pressed()), this, SLOT(pushButton_4_pressed()));

	////�˵����ؼ�
	this->ui.a_analysis->setEnabled(false);
	this->ui.a_database->setEnabled(false);
	this->ui.a_input->setEnabled(false);
	this->ui.a_admin->setEnabled(false);
	this->ui.a_main->setEnabled(false);
	connect(this->ui.a_login, SIGNAL(triggered()), this, SLOT(switchPage()));
	connect(this->ui.a_registe, SIGNAL(triggered()), this, SLOT(switchPage()));
	connect(this->ui.a_input, SIGNAL(triggered()), this, SLOT(switchPage()));
	connect(this->ui.a_analysis, SIGNAL(triggered()), this, SLOT(switchPage()));
	connect(this->ui.a_database, SIGNAL(triggered()), this, SLOT(switchPage()));
	connect(this->ui.a_admin, SIGNAL(triggered()), this, SLOT(switchPage()));
	connect(this->ui.a_main, SIGNAL(triggered()), this, SLOT(switchPage()));

	////loginҳ��ؼ�
	this->ui.le_password->setEchoMode(QLineEdit::Password);//�����Ǻ� 
	this->ui.pB_login->setShortcut(Qt::Key_Enter);//����ĸ���س������¼��ť����һ��
	this->ui.pB_login->setShortcut(Qt::Key_Return);
	connect(this->ui.pB_login, SIGNAL(pressed()), this, SLOT(pB_login_clicked()));
	connect(this->ui.pB_reset_2, SIGNAL(pressed()), this, SLOT(pB_reset_clicked()));

	////registeҳ��ؼ�
	this->ui.le_password_2->setEchoMode(QLineEdit::Password);
	this->ui.le_password_3->setEchoMode(QLineEdit::Password);
	this->ui.pB_registe->setShortcut(Qt::Key_Enter);
	this->ui.pB_registe->setShortcut(Qt::Key_Return);
	connect(this->ui.pB_registe, SIGNAL(pressed()), this, SLOT(pB_registe_clicked()));
	connect(this->ui.pB_reset, SIGNAL(pressed()), this, SLOT(pB_reset_clicked()));
	connect(this->ui.cB_showPassword, SIGNAL(clicked()), this, SLOT(cB_showPassword_clicked()));


	////adminҳ��ؼ�
	tableViewerInit();
	connect(this->ui.radioButton_14, SIGNAL(clicked()), this, SLOT(comoBoxEditable()));
	connect(this->ui.radioButton_15, SIGNAL(clicked()), this, SLOT(comoBoxEditable()));
	connect(this->ui.radioButton_16, SIGNAL(clicked()), this, SLOT(comoBoxEditable()));
	connect(this->ui.radioButton_13, SIGNAL(clicked()), this, SLOT(comoBoxEditable()));
	connect(this->ui.pushButton_6, SIGNAL(pressed()), this, SLOT(query()));
	connect(this->ui.pushButton_5, SIGNAL(pressed()), this, SLOT(reset()));
	connect(this->ui.pushButton_7, SIGNAL(pressed()), this, SLOT(drop()));
	connect(this->ui.pushButton_8, SIGNAL(pressed()), this, SLOT(addin()));


	////dataInputҳ��ؼ�
	ui.pB_dataImport->setEnabled(false);
	ui.cB_changeKeyPoints->setEnabled(false);
	this->ui.groupBox_2->setEnabled(false);
	this->ui.pB_dataRenew->setEnabled(false);
	connect(this->ui.pB_selectPatient, SIGNAL(pressed()), this, SLOT(selectPatient()));
	connect(this->ui.pB_dataImport, SIGNAL(pressed()), this, SLOT(pB_dataImport_clicked()));
	connect(this->ui.pB_dataRenew, SIGNAL(pressed()), this, SLOT(pB_dataRenew_clicked()));
	connect(this->ui.cB_changeKeyPoints, SIGNAL(clicked()), this, SLOT(cB_changeKeyPoints_clicked()));
	connect(this->ui.radioButton, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_2, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_3, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_4, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_5, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_6, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_7, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_8, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_9, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_10, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_11, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.radioButton_12, SIGNAL(clicked()), this, SLOT(radioButton_clicked()));
	connect(this->ui.pB_cancelRePick_2, SIGNAL(clicked()), this, SLOT(pB_cancelRePick_clicked()));
	connect(this->ui.pB_confirmRePick, SIGNAL(clicked()), this, SLOT(pB_confirmRePick_clicked()));
	////��ģ�͵���ģ��������߳�
	workerThread = new WorkerThread;
	thread = new QThread;
	//thread->setPriority(QThread::LowPriority);//�����߳����ȼ�
	workerThread->moveToThread(thread);
	connect(this, SIGNAL(ready()), workerThread, SLOT(modelInput()), Qt::QueuedConnection);
	connect(workerThread, SIGNAL(finishedInput()), this, SLOT(uiRenew()));


	////dataAnalysisҳ��ؼ�
	//ui.pB_exportPDF->setEnabled(false);
	connect(this->ui.pB_exportPDF, SIGNAL(pressed()), this, SLOT(pB_exportPDF_clicked()));
	connect(this->ui.pB_dataAnalysis, SIGNAL(pressed()), this, SLOT(popNewWindow()));
	connect(this->ui.pB_dataAnalysis_2, SIGNAL(pressed()), this, SLOT(popNewWindow_2()));
	////��ģ�͵���ģ��������߳�
	workerThread_2 = new WorkerThread_2;
	workerThread_2->moveToThread(thread);
	connect(this, SIGNAL(ready_2()), workerThread_2, SLOT(modelInput()), Qt::QueuedConnection);
	connect(workerThread_2, SIGNAL(finished()), this, SLOT(finishInput()));
	workerThread_3 = new WorkerThread_3;
	workerThread_3->moveToThread(thread);
	connect(this, SIGNAL(ready_3()), workerThread_3, SLOT(modelInput()), Qt::QueuedConnection);
	connect(workerThread_3, SIGNAL(finished_2()), this, SLOT(finishInput_2()));
	thread->start();

}

MainWindow::~MainWindow()
{
	//delete ui;
}


////////////////////////////////////////////////////////////////////////////
// ������initialVtkWidget
// ��������ʼ��QVTKWidget�ؼ�
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::initialVtkWidget()
{
	John.cUpRight.reset(new pcl::PointCloud<pcl::PointXYZ>);
	John.keyPtr.reset(new pcl::PointCloud<pcl::PointXYZ>);
	John.correctedPtr.reset(new pcl::PointCloud<pcl::PointXYZ>);
	John.cloud.reset(new pcl::PointCloud<pcl::PointXYZ>);
	
	///////////////////////viewer/////////////////////////
	John.viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	John.viewer->initCameraParameters();
	
	int vleft(0);
	John.viewer->createViewPort(0.0, 0.0, 0.5, 1.0, vleft);
	John.viewer->setBackgroundColor(0.7, 0.7, 0.7, vleft);
	John.viewer->setCameraPosition(0, 0, 500, 0, 1, 0);
	
	int vright(1);
	John.viewer->createViewPort(0.5, 0.0, 1.0, 1.0, vright);
	John.viewer->setBackgroundColor(0.8, 0.8, 0.8, vright);
	John.viewer->createViewPortCamera(vright);
	John.viewer->setCameraPosition(800, 0, 0, 0, 1, 0, vright);
	
	//John.viewer->addPointCloud(John.cloud, "cloud");
	ui.qvtkWidget->SetRenderWindow(John.viewer->getRenderWindow());
	John.viewer->setupInteractor(ui.qvtkWidget->GetInteractor(), ui.qvtkWidget->GetRenderWindow());
	ui.qvtkWidget->update();


	///////////////////////viewer_2/////////////////////////
	John.viewer_2.reset(new pcl::visualization::PCLVisualizer("viewer_2", false));
	John.viewer_2->initCameraParameters();

	//int vleft(0);
	John.viewer_2->createViewPort(0.0, 0.0, 0.5, 1.0, vleft);
	John.viewer_2->setBackgroundColor(0.7, 0.7, 0.7, vleft);
	John.viewer_2->setCameraPosition(0, 0, 500, 0, 1, 0);

	//int vright(1);
	John.viewer_2->createViewPort(0.5, 0.0, 1.0, 1.0, vright);
	John.viewer_2->setBackgroundColor(0.8, 0.8, 0.8, vright);
	John.viewer_2->createViewPortCamera(vright);
	John.viewer_2->setCameraPosition(800, 0, 0, 0, 1, 0, vright);
	John.viewer_2->addCoordinateSystem(20.0, "Coordinate_2");

	//John.viewer->addPointCloud(John.cloud, "cloud");
	ui.qvtkWidget_2->SetRenderWindow(John.viewer_2->getRenderWindow());
	John.viewer_2->setupInteractor(ui.qvtkWidget_2->GetInteractor(), ui.qvtkWidget_2->GetRenderWindow());
	ui.qvtkWidget_2->update();


	///////////////////////viewer_3/////////////////////////
	John.viewer_3.reset(new pcl::visualization::PCLVisualizer("viewer_3", false));
	John.viewer_3->initCameraParameters();

	//int vleft(0);
	John.viewer_3->createViewPort(0.0, 0.0, 0.5, 1.0, vleft);
	John.viewer_3->setBackgroundColor(0.7, 0.7, 0.7, vleft);
	John.viewer_3->setCameraPosition(0, 0, 500, 0, 1, 0);

	//int vright(1);
	John.viewer_3->createViewPort(0.5, 0.0, 1.0, 1.0, vright);
	John.viewer_3->setBackgroundColor(0.8, 0.8, 0.8, vright);
	John.viewer_3->createViewPortCamera(vright);
	John.viewer_3->setCameraPosition(800, 0, 0, 0, 1, 0, vright);
	John.viewer_3->addCoordinateSystem(20.0, "Coordinate_3");

	//John.viewer->addPointCloud(John.cloud, "cloud");
	ui.qvtkWidget_3->SetRenderWindow(John.viewer_3->getRenderWindow());
	John.viewer_3->setupInteractor(ui.qvtkWidget_3->GetInteractor(), ui.qvtkWidget_3->GetRenderWindow());
	ui.qvtkWidget_3->update();
}

////////////////////////////////////////////////////////////////////////////
// ������databaseInit
// ��������ʼ�����ݿ�����
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::databaseInit()
{
	db = QSqlDatabase::addDatabase("QMYSQL");
	//��ȡmysql���������ļ�
	QSettings *m_IniFile = new QSettings("Head3D.ini", QSettings::IniFormat);
	QString hostName = m_IniFile->value("/mysql-connect/HostName").toString();
	QString databaseName = m_IniFile->value("/mysql-connect/DatabaseName").toString();
	QString userName = m_IniFile->value("/mysql-connect/UserName").toString();
	QString password = m_IniFile->value("/mysql-connect/Password").toString();

	//db.setHostName("%1").arg(hostName);
	//db.setDatabaseName("%1").arg(databaseName);
	//db.setUserName("%1").arg(userName);
	//db.setPassword("%1").arg(password);

	db.setHostName(hostName);
	db.setDatabaseName(databaseName);
	db.setUserName(userName);
	db.setPassword(password);

	QSqlQuery sql_query(db);
	//sql_query.exec("CREATE database if not exists head3d set utf8");
	sql_query.exec("alter database head3d character set utf8");
	if (!db.open())
	{
		QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���ݿ��ʧ�ܣ�"), QMessageBox::Yes);
		return;
	}
	else
	{
		sql_query.exec("use head3d");
		QString create_sql1 = "CREATE TABLE if not exists Doctor (id int primary key AUTO_INCREMENT, name varchar(40), pswd varchar(40))";//password�����ݿ�ؼ���
		sql_query.prepare(create_sql1);
		if (!sql_query.exec(create_sql1))
		{
			QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("��1����ʧ�ܣ�"), QMessageBox::Yes);
			return;
		}
		QString create_sql2 = "CREATE TABLE if not exists Patient (id int primary key AUTO_INCREMENT, userID varchar(40), name varchar(40), sex varchar(40), birthday date, time datetime, size0 varchar(40), size1 varchar(40), size2 varchar(40), size3 varchar(40), size4 varchar(40), size5 varchar(40), size6 varchar(40), size7 varchar(40), size8 varchar(40), size9 varchar(40))";
		sql_query.prepare(create_sql2);
		if (!sql_query.exec(create_sql2))
		{
			QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("��2����ʧ�ܣ�"), QMessageBox::Yes);
			return;
		}
		QString create_sql3 = "CREATE TABLE if not exists PatientInformation (id int primary key AUTO_INCREMENT, userID varchar(40), name varchar(40), sex varchar(40), birthday date, registertime datetime)";
		sql_query.prepare(create_sql3);
		if (!sql_query.exec(create_sql3))
		{
			QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("��3����ʧ�ܣ�"), QMessageBox::Yes);
			return;
		}
	}
}


/////////////////////////////////�˵���///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������switchPage
// �������˵���ҳ���л�
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::switchPage()
{
	QAction *action = qobject_cast<QAction*>(sender());//�õ����µİ�ť��ָ��
	QPushButton *button = qobject_cast<QPushButton*>(sender());
	if (action == ui.a_login)
	{
		databaseInit();
		this->ui.le_password->setEchoMode(QLineEdit::Password);//�����Ǻ�  
		ui.stackedWidget->setCurrentIndex(0);//���ݰ��µ�button��������ʾ��Ӧ��ҳ��	
	}
	else if (action == ui.a_registe)
	{
		databaseInit();
		this->ui.le_password_2->setEchoMode(QLineEdit::Password);
		this->ui.le_password_3->setEchoMode(QLineEdit::Password);
		ui.stackedWidget->setCurrentIndex(1);
	}
	else if (action == ui.a_input)
	{
		databaseInit();
		ui.stackedWidget->setCurrentIndex(2);
	}
	else if (action == ui.a_analysis)
	{
		databaseInit();
		ui.stackedWidget->setCurrentIndex(3);
	}
	else if (action == ui.a_main)
	{
		databaseInit();
		ui.stackedWidget->setCurrentIndex(4);
	}
	else if (action == ui.a_admin)
	{
		databaseInit();
		ui.stackedWidget->setCurrentIndex(5);
		tableViewerInit();
	}
}

/////////////////////////////////��ӭҳ��///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������pushButton_pressed
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pushButton_4_pressed()
{
	ui.stackedWidget->setCurrentIndex(5);
	databaseInit();
	tableViewerInit();

}

////////////////////////////////////////////////////////////////////////////
// ������pushButton_pressed
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pushButton_pressed()
{
	ui.stackedWidget->setCurrentIndex(2);
	databaseInit();

}

////////////////////////////////////////////////////////////////////////////
// ������pushButton_2_pressed
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pushButton_2_pressed()
{
	ui.stackedWidget->setCurrentIndex(3);
	databaseInit();

}

////////////////////////////////////////////////////////////////////////////
// ������pushButton_3_pressed
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pushButton_3_pressed()
{
	QMessageBox::warning(this, QStringLiteral("��ʾ"), QStringLiteral("ȷ���˳�ϵͳ��"), QMessageBox::Yes);
	QApplication::exit();
}

/////////////////////////////////��¼ҳ��///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������pB_reset_clicked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_reset_clicked()
{

	this->ui.le_account_2->clear();
	this->ui.le_password_2->clear();
	this->ui.le_password_3->clear();
	this->ui.le_account_2->setFocus();
	this->ui.le_account->clear();
	this->ui.le_account->setFocus();
	this->ui.le_password->clear();
}

////////////////////////////////////////////////////////////////////////////
// ������pB_login_clicked()
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_login_clicked()
{
	QSqlQuery sql_query(db);
	sql_query.exec("use head3d");
	if (!db.open())
	{
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("���ݿ��ʧ�ܣ�"), QMessageBox::Yes);
		return;
	}
	else
	{
		QString username = this->ui.le_account->text();
		QString password = this->ui.le_password->text();
		QString select_sql = "select * from Doctor";
		if (!sql_query.exec(select_sql))
		{
			QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��������"), QMessageBox::Yes);
			return;
		}
		else
		{
			while (sql_query.next())
			{
				QString mname = sql_query.value("name").toString();
				QString mpass = sql_query.value("pswd").toString();
				if (username == mname && password == mpass)
				{
					//QMessageBox::warning(this, QStringLiteral("��ʾ"), QStringLiteral("��¼�ɹ���"), QMessageBox::Yes);
					admin = this->ui.le_account->text();
					ui.stackedWidget->setCurrentIndex(4);
					ui.groupBox_8->setEnabled(false);
					this->ui.a_analysis->setEnabled(true);
					this->ui.a_database->setEnabled(true);
					this->ui.a_input->setEnabled(true);
					this->ui.a_admin->setEnabled(true);
					this->ui.a_main->setEnabled(true);
					this->ui.le_account->clear();
					this->ui.le_password->clear();
					//this->ui.le_account->setFocus();

					return;
				}
			}

			//һ��ʼ���Ϊ��
			QMessageBox::warning(this, QStringLiteral("��ʾ"), QStringLiteral("��������ȷ���û��������룡"), QMessageBox::Yes);
			this->ui.le_account->clear();
			this->ui.le_password->clear();
			this->ui.le_account->setFocus();
			return;
		}
	}
}

/////////////////////////////////ע��ҳ��///////////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������pB_registe_clicked()
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_registe_clicked()
{
	QSqlQuery sql_query(db);
	sql_query.exec("use head3d");
	if (!db.open())
	{
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("���ݿ��ʧ�ܣ�"), QMessageBox::Yes);
		return;
	}
	else
	{
		QString username = this->ui.le_account_2->text();
		QString password1 = this->ui.le_password_2->text();
		QString password2 = this->ui.le_password_3->text();
		QString select_sql = "select name from Doctor";
		sql_query.prepare(select_sql);
		if (!sql_query.exec(select_sql))
		{
			QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��������"), QMessageBox::Yes);
			return;
		}
		else if (username == "")
		{
			QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("�û�������Ϊ�գ�"), QMessageBox::Yes);
			return;
		}
		else if (password1 == "")
		{
			QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("���벻��Ϊ�գ�"), QMessageBox::Yes);
			return;
		}
		else if (password2 == "")
		{
			QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��ȷ�����룡"), QMessageBox::Yes);
			return;
		}
		else
		{
			while (sql_query.next())
			{
				QString mname = sql_query.value("name").toString();
				if (username == mname)
				{
					QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("���û�����ע�ᣡ"), QMessageBox::Yes);
					return;
				}
			}

			if (password1 != password2)
			{
				QMessageBox::warning(this, QStringLiteral("��ʾ"), QStringLiteral("���벻һ�£�"), QMessageBox::Yes);
				return;
			}
			else
			{
				QString insert_sql = QObject::tr("insert into Doctor values('%1','%2','%3')").arg(NULL).arg(username).arg(password2);
				sql_query.prepare(insert_sql);
				if (!sql_query.exec(insert_sql))
				{
					QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("ע��ʧ�ܣ�"), QMessageBox::Yes);
					pB_reset_clicked();
					return;
				}
				else
				{
					QMessageBox::warning(this, QStringLiteral("��ʾ"), QStringLiteral("ע��ɹ���"), QMessageBox::Yes);
					ui.stackedWidget->setCurrentIndex(0);
					this->ui.le_account_2->clear();
					this->ui.le_password_2->clear();
					this->ui.le_password_3->clear();

					return;
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////
// ������cB_showPassword_checked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::cB_showPassword_clicked()
{
	if (ui.cB_showPassword->isChecked() == true)
	{
		this->ui.le_password_2->setEchoMode(QLineEdit::Normal);
		this->ui.le_password_3->setEchoMode(QLineEdit::Normal);
	}
	else
	{
		this->ui.le_password_2->setEchoMode(QLineEdit::Password);
		this->ui.le_password_3->setEchoMode(QLineEdit::Password);
	}
}

/////////////////////////////������Ϣ�������///////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������tableViewerInit
// ��������ʼ��TableViewer
// ���룺Null
// �����Null
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::tableViewerInit()
{
	databaseInit();

	QSqlQuery sql_query(db);
	if (!db.open())
	{
		QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���ݿ��ʧ�ܣ�"), QMessageBox::Yes);
		return;
	}
	else
	{
		//����������Ŀ
		QStringList strings;
		QCompleter *completer;
		sql_query.exec("SELECT name FROM patientinformation");
		while (sql_query.next()) {
			QString name = sql_query.value("name").toString();
			strings.append(name);
		}
		//ȥ��
		for (int i = 0; i < strings.count(); i++)
		{
			for (int k = i + 1; k < strings.count(); k++)
			{
				if (strings.at(i) == strings.at(k))
				{
					strings.removeAt(k);
					k--;
				}
			}
		}
		completer = new QCompleter(strings, this);
		ui.comboBox_3->clear();
		ui.comboBox_3->addItems(strings);
		ui.comboBox_3->setCompleter(completer);
		ui.comboBox_3->setCurrentText("");

		QStringList strings_2;
		sql_query.exec("SELECT userid FROM patientInformation");
		while (sql_query.next()) {
			QString userID = sql_query.value("userid").toString();
			strings_2.append(userID);
		}
		for (int i = 0; i < strings_2.count(); i++)
		{
			for (int k = i + 1; k < strings_2.count(); k++)
			{
				if (strings_2.at(i) == strings_2.at(k))
				{
					strings_2.removeAt(k);
					k--;
				}
			}
		}
		completer = new QCompleter(strings_2, this);
		ui.comboBox_4->clear();
		ui.comboBox_4->addItems(strings_2);
		ui.comboBox_4->setCompleter(completer);
		ui.comboBox_4->setCurrentText("");

		ui.comboBox_3->setEnabled(false);
		ui.comboBox_4->setEnabled(false);
		ui.dateEdit->setEnabled(false);
		ui.dateEdit_2->setEnabled(false);
		ui.dateEdit_3->setEnabled(false);
		ui.dateEdit_5->setEnabled(false);

		//���ݿ�����ʾ
		model = new QSqlTableModel(this);
		model->setTable("PatientInformation");
		model->setSort(1, Qt::AscendingOrder);
		// model->setHeaderData(0, Qt::Horizontal, QStringLiteral("���߱��    "));
		model->setHeaderData(1, Qt::Horizontal, QStringLiteral("���߱��"));
		model->setHeaderData(2, Qt::Horizontal, QStringLiteral("��������"));
		model->setHeaderData(3, Qt::Horizontal, QStringLiteral("�����Ա�"));
		model->setHeaderData(4, Qt::Horizontal, QStringLiteral("���߳�������"));
		model->setHeaderData(5, Qt::Horizontal, QStringLiteral("¼��ʱ��"));

		ui.tableView->setModel(model);
		ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView->setColumnHidden(0, true);//���ص�һ��
		//ui.tableView->setColumnHidden(5, true);
		//ui.tableView->setColumnHidden(6, true);
		//ui.tableView->setColumnHidden(7, true);
		//ui.tableView->setColumnHidden(8, true);
		//ui.tableView->setColumnHidden(9, true);
		//ui.tableView->setColumnHidden(10, true);
		//ui.tableView->setColumnHidden(11, true);
		//ui.tableView->setColumnHidden(12, true);
		//ui.tableView->setColumnHidden(13, true);
		//ui.tableView->setColumnHidden(14, true);
		//ui.tableView->setColumnHidden(15, true);

		ui.tableView->resizeColumnsToContents();
		ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

		QHeaderView *header = ui.tableView->horizontalHeader();
		header->setStretchLastSection(true);//���ó�������
		header->setSectionResizeMode(QHeaderView::ResizeToContents);
		//header->setSectionResizeMode(QHeaderView::Stretch);//�����п�
		ui.tableView->show();
		model->select(); //��ѯ���ű�
	}
}


////////////////////////////////////////////////////////////////////////////
// ������query
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::query()
{
	QString select;
	select = "1=1";//������û��ʵ�����壬��֤��������еġ�and����Ч
	if (ui.radioButton_14->isChecked())
	{
		select = QString("name = '%1'").arg(ui.comboBox_3->currentText());
		model->setFilter(select);
		model->select();//������Ϣ��ͬʱ������ͼ
		select = "";
	}
	else if (ui.radioButton_15->isChecked())
	{
		select = QString("userID = '%1'").arg(ui.comboBox_4->currentText());
		model->setFilter(select);
		model->select();//������Ϣ��ͬʱ������ͼ
		select = "";
	}
	else if (ui.radioButton_16->isChecked())
	{
		select = QString("birthday >= '%1' and birthday <= '%2'").arg(ui.dateEdit->date().toString("yyyy-MM-dd")).arg(ui.dateEdit_2->date().toString("yyyy-MM-dd"));
		model->setFilter(select);
		model->select();//������Ϣ��ͬʱ������ͼ
		select = "";
	}
	else if (ui.radioButton_13->isChecked())
	{
		select = QString("registertime >= '%1' and registertime <= '%2'").arg(ui.dateEdit_3->date().toString("yyyy-MM-dd")).arg(ui.dateEdit_5->date().toString("yyyy-MM-dd"));
		model->setFilter(select);
		model->select();//������Ϣ��ͬʱ������ͼ
		select = "";
	}
}

////////////////////////////////////////////////////////////////////////////
// ������reset
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::reset()
{
	ui.radioButton_14->setChecked(false);
	ui.radioButton_15->setChecked(false);
	ui.radioButton_16->setChecked(false);
	ui.radioButton_13->setChecked(false);
	ui.comboBox_3->setCurrentText("");
	ui.comboBox_4->setCurrentText("");
	ui.comboBox_3->setEnabled(false);
	ui.comboBox_4->setEnabled(false);
	ui.dateEdit->setEnabled(false);
	ui.dateEdit_2->setEnabled(false);
	ui.dateEdit_3->setEnabled(false);
	ui.dateEdit_5->setEnabled(false);

	QString select = "1=1";
	model->setFilter(select);
	model->select();
}

////////////////////////////////////////////////////////////////////////////
// ������comoBoxEditable
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::comoBoxEditable()
{
	QTime reachTime = QTime::currentTime().addMSecs(100);
	while (QTime::currentTime() < reachTime)
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}
	if (ui.radioButton_14->isChecked() == true)
	{
		ui.comboBox_3->setEnabled(true);
	}
	else
	{
		ui.comboBox_3->setEnabled(false);
	}


	if (ui.radioButton_15->isChecked() == true)
	{
		ui.comboBox_4->setEnabled(true);
	}
	else
	{
		ui.comboBox_4->setEnabled(false);
	}


	if (ui.radioButton_16->isChecked() == true)
	{
		ui.dateEdit->setEnabled(true);
		ui.dateEdit_2->setEnabled(true);
		ui.dateEdit->setCalendarPopup(true);
		ui.dateEdit_2->setCalendarPopup(true);
	}
	else
	{
		ui.dateEdit->setEnabled(false);
		ui.dateEdit_2->setEnabled(false);
	}

	if (ui.radioButton_13->isChecked() == true)
	{
		ui.dateEdit_3->setEnabled(true);
		ui.dateEdit_5->setEnabled(true);
		ui.dateEdit_3->setCalendarPopup(true);
		ui.dateEdit_5->setCalendarPopup(true);
	}
	else
	{
		ui.dateEdit_3->setEnabled(false);
		ui.dateEdit_5->setEnabled(false);
	}
}

////////////////////////////////////////////////////////////////////////////
// ������drop
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::drop()
{
	QMessageBox::StandardButton rb = QMessageBox::question(NULL, QStringLiteral("����"), QStringLiteral("ȷ��ɾ��������Ϣ��"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	if (rb == QMessageBox::No)
		return;
	if (rb == QMessageBox::Yes)
	{
		int curRow = ui.tableView->currentIndex().row();
		QString name = model->record(curRow).value("name").toString();

		QSqlQuery drop_sql(db);
		QString drop = QObject::tr("DELETE FROM patientInformation WHERE name = '%1'").arg(name);
		drop_sql.prepare(drop);
		drop_sql.exec(drop);
		drop = QObject::tr("DELETE FROM patient WHERE name = '%1'").arg(name);
		drop_sql.prepare(drop);
		drop_sql.exec(drop);

		tableViewerInit();
		//model->select();
	}
}

////////////////////////////////////////////////////////////////////////////
// ������addin
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::addin()
{
	//QMessageBox::StandardButton rb = QMessageBox::question(NULL, QStringLiteral("����"), QStringLiteral("ȷ������»��ߣ�"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	//if (rb == QMessageBox::No)
	//	return;
	//if (rb == QMessageBox::Yes)
	//{
		QString name = this->ui.lineEdit_2->text();
		QString sex = this->ui.comboBox_5->currentText();
		QDate birth = this->ui.dateEdit_4->date();
		QString birthday = birth.toString("yyyy-MM-dd");
		QDateTime today = QDateTime::currentDateTime();
		QString todayTime = today.toString("yyyy-MM-dd HH:mm:ss");

		QSqlQuery addin_sql(db);
		QSqlQuery sql_query(db);

		QString userID;
		int nameResist = 0;

		if (!db.open())
		{
			QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("���ݿ��ʧ�ܣ�"), QMessageBox::Yes);
			return;
		}
		else
		{
			QString select_sql = "select * from patientInformation";
			sql_query.prepare(select_sql);
			if (!sql_query.exec(select_sql))
			{
				QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��������"), QMessageBox::Yes);
				return;
			}
			else
			{
				while (sql_query.next())
				{
					QString mname = sql_query.value("name").toString();
					//QString userID = sql_query.value("userid").toString();
					if (name == mname)
					{
						QMessageBox::StandardButton rb = QMessageBox::question(NULL, QStringLiteral("����"), QStringLiteral("�û����ѵǼǣ�ȷ�ϸ��ǣ�"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
						if (rb == QMessageBox::No)
							return;
						if (rb == QMessageBox::Yes)
						{
							QSqlQuery drop_sql(db);
							userID = sql_query.value("userid").toString();
							QString id = sql_query.value("id").toString();
							QString drop = QObject::tr("DELETE FROM patientInformation WHERE id = '%1'").arg(id);
							drop_sql.prepare(drop);
							drop_sql.exec(drop);

							nameResist = 1;
							break;
						}
					}
				}
				if (nameResist == 0)
				{
					int uID = 0;
					sql_query.exec("select userID from patientInformation");
					while (sql_query.next())
					{
						if (uID < sql_query.value("userID").toInt())
							uID = sql_query.value("userID").toInt();
					}
					uID++;
					userID = QString("%1").arg(uID, 4, 10, QLatin1Char('0'));
				}

				QString insert_sql = QObject::tr("insert into PatientInformation (id, userID, name, sex, birthday, registertime) values('%1','%2','%3','%4','%5','%6')").arg(NULL).arg(userID).arg(name).arg(sex).arg(birthday).arg(todayTime);
				sql_query.prepare(insert_sql);
				if (!sql_query.exec(insert_sql))
				{
					QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��������ʧ�ܣ�"), QMessageBox::Yes);
					return;
				}
				else
				{
					QMessageBox::warning(this, QStringLiteral("��ʾ"), QStringLiteral("�������ݳɹ���"), QMessageBox::Yes);
					//model->select();//ˢ��tableviewer
					tableViewerInit();
					return;
				}				
			}
		}
	//}	
}


/////////////////////////////����ģ�͵���ҳ��///////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������selectPatient
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::selectPatient()
{
	dataselect_2 = new dataSelect_2(this);
	dataselect_2->flag = 2;
	dataselect_2->show();
	dataselect_2->setWindowTitle(QStringLiteral("ѡ����"));
	connect(dataselect_2, SIGNAL(selectPatient()), this, SLOT(inputPatientInformation()));
}

////////////////////////////////////////////////////////////////////////////
// ������inputPatientInformation
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::inputPatientInformation()
{
	dataselect_2->close();
	ui.pB_dataImport->setEnabled(true);


	int curIndex = this->dataselect_2->curIndex;
	QString name = this->dataselect_2->model->record(curIndex).value("name").toString();
	QString sex = this->dataselect_2->model->record(curIndex).value("sex").toString();
	QString id = this->dataselect_2->model->record(curIndex).value("userid").toString();

	QDate birth = this->dataselect_2->model->record(curIndex).value("birthday").toDate();
	QString birthday = birth.toString("yyyy-MM-dd");
	birthStr = birthday;//����ҳ�汣���������
	QDate currentDate = QDate::currentDate();
	int age = currentDate.year() - birth.year() + 1;
	QString Age = QString::number(age);

	ui.lineEdit_35->setText(name);
	ui.lineEdit_36->setText(sex);
	ui.lineEdit_37->setText(id);
	ui.lineEdit_38->setText(Age);
	QDateTime currentDateTime = QDateTime::currentDateTime();
	ui.lineEdit_39->setText(currentDateTime.toString("yyyy-MM-dd HH:mm:ss"));
}

////////////////////////////////////////////////////////////////////////////
// ������pB_dataImport_clicked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_dataImport_clicked()
{
	databaseInit();
	thread->start();

	//��ȡ���ļ���
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open PointCloud"), ".", tr("STL files(*.stl)"));
	QFileInfo fileinfo = QFileInfo(fileName);
	QString fileSuffix = fileinfo.suffix();
	//std::string filename = std::string((const char*)fileName.toLocal8Bit());//���뷽ʽת�����ܹ�ʶ������
	John.filename = std::string((const char*)fileName.toLocal8Bit());

	if (!fileName.isEmpty())
	{
		//qDebug() << "send thread ID:" << QThread::currentThreadId() << '\n';
		
		//��ʼ��
		//John.cUpRight.reset(new pcl::PointCloud<pcl::PointXYZ>);
		//John.keyPtr.reset(new pcl::PointCloud<pcl::PointXYZ>);
		John.correctedPtr.reset(new pcl::PointCloud<pcl::PointXYZ>);
		//John.viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
		emit ready();

		//������ʾ
		progressDlg = new QProgressDialog(this);//��ʵ��һ�����Ѿ���ʼ��ʾ��������
		progressDlg->setWindowModality(Qt::WindowModal);//���θ�����
		//progressDlg->setMinimumDuration(0);
		//progressDlg->setMaximum(100);
		progressDlg->setMaximum(0);
		progressDlg->setMinimum(0);
		//progressDlg->setValue(0);
		progressDlg->setAttribute(Qt::WA_DeleteOnClose, true);//�رմ��ڽ���ɾ��
		progressDlg->setWindowTitle(QStringLiteral("������"));
		progressDlg->setLabelText(QStringLiteral("���Ժ�ģ�͵�����Ҫ��Լ1����..."));
		progressDlg->setCancelButton(0);
		Qt::WindowFlags flags = Qt::Dialog;
		flags |= Qt::WindowCloseButtonHint;
		progressDlg->setWindowFlags(flags);
		progressDlg->show();
		//progressDlg->setCancelButtonText(QStringLiteral("ȡ��"));
		////progressDlg->setRange(0, 100);//�����˽���������
		//progressDlg->show();
		//if (progressDlg->wasCanceled())
		//{
		//	thread->quit();
		//	thread->wait();
		//	delete workerThread;
		//	progressDlg->close();
		//	//this->ui.pB_dataRenew->setEnabled(false);

		//	return;
		//}
	}

}

////////////////////////////////////////////////////////////////////////////
// ������uiRenew
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::uiRenew()
{
	thread->quit();
	thread->wait();
	//delete workerThread;

	progressDlg->close();

	//John.viewer.reset(new pcl::visualization::PCLVisualizer("viewer", false));
	//John.viewer->initCameraParameters();

	//int vleft(0);
	//John.viewer->createViewPort(0.0, 0.0, 0.5, 1.0, vleft);
	//John.viewer->setBackgroundColor(0.7, 0.7, 0.7, vleft);
	//John.viewer->setCameraPosition(0, 0, 500, 0, 1, 0);

	//int vright(1);
	//John.viewer->createViewPort(0.5, 0.0, 1.0, 1.0, vright);
	//John.viewer->setBackgroundColor(0.8, 0.8, 0.8, vright);
	//John.viewer->createViewPortCamera(vright);
	//John.viewer->setCameraPosition(800, 0, 0, 0, 1, 0, vright);

	////John.viewer->addPointCloud(John.cloud, "cloud");
	//ui.qvtkWidget->SetRenderWindow(John.viewer->getRenderWindow());
	//John.viewer->setupInteractor(ui.qvtkWidget->GetInteractor(), ui.qvtkWidget->GetRenderWindow());
	//ui.qvtkWidget->update();

	John.showModel(John.mesh, John.viewer);
	ui.qvtkWidget->update();
	John.showKeyPoints(John.keyPtr, John.viewer);
	ui.qvtkWidget->update();

	string size0 = std::to_string((int)(John.computeHeadCircumference(John.cUpRight, John.keyPtr->points.at(4))));//ͷΧ
	string size1 = std::to_string((int)(fabs(John.keyPtr->points.at(1).y - John.keyPtr->points.at(3).y)));//ͷ����������
	string size2 = std::to_string((int)(fabs(John.keyPtr->points.at(7).x - John.keyPtr->points.at(8).x)));//����
	string size3 = std::to_string((int)(fabs(John.keyPtr->points.at(9).x - John.keyPtr->points.at(10).x)));//�����
	string size4 = std::to_string((int)(John.computeDistance3D(John.keyPtr->points.at(4), John.keyPtr->points.at(0))));//ü�ĵ�����Ե����
	string size5 = std::to_string((int)(fabs(John.keyPtr->points.at(0).z - John.keyPtr->points.at(4).z)));//ü�ĵ�����Եˮƽ����
	string size6 = std::to_string((int)(fabs(John.keyPtr->points.at(5).y - John.keyPtr->points.at(6).y)));//����
	string size7 = std::to_string((int)(fabs(John.keyPtr->points.at(0).z - John.keyPtr->points.at(2).z)));//������ߵ㵽�Ǽ����
	string size8 = std::to_string((int)(fabs(John.keyPtr->points.at(4).z - John.keyPtr->points.at(2).z)));//ü�ǵ�������ߵ����

	this->ui.lineEdit_5->setText(QString::fromStdString(size0));
	this->ui.lineEdit_6->setText(QString::fromStdString(size1));
	this->ui.lineEdit_7->setText(QString::fromStdString(size2));
	this->ui.lineEdit_9->setText(QString::fromStdString(size3));
	this->ui.lineEdit_10->setText(QString::fromStdString(size4));
	this->ui.lineEdit_8->setText(QString::fromStdString(size5));
	this->ui.lineEdit_13->setText(QString::fromStdString(size6));
	this->ui.lineEdit_11->setText(QString::fromStdString(size7));
	this->ui.lineEdit_12->setText(QString::fromStdString(size8));
	this->ui.lineEdit_14->setText(QStringLiteral("�޶�ͷ��ߵ�"));

	QDate time = QDate::currentDate();
	//this->ui.dateEdit->setDate(time);

	ui.cB_changeKeyPoints->setEnabled(true);
	ui.groupBox_4->setEnabled(true);

	//progressDlg->close();
	this->ui.pB_dataRenew->setEnabled(true);

	//John.correctedPtr.reset(new pcl::PointCloud<pcl::PointXYZ>);
	John.correctedPtr->clear();
	John.correctedPtr->resize(John.keyPtr->size());
	for (size_t i = 0; i < John.keyPtr->points.size(); i++) {
		John.correctedPtr->points[i].x = John.keyPtr->points[i].x;
		John.correctedPtr->points[i].y = John.keyPtr->points[i].y;
		John.correctedPtr->points[i].z = John.keyPtr->points[i].z;
	}
	//pcl::copyPointCloud(*(John.keyPtr), *(John.correctedPtr));//���ƺ���������ƣ����ܵĴ���ԭ��

	ui.cB_changeKeyPoints->setEnabled(true);
	ui.pB_dataRenew->setEnabled(true);
}

////////////////////////////////////////////////////////////////////////////
// ������cB_changeKeyPoints_checked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::cB_changeKeyPoints_clicked()
{
	if (this->ui.cB_changeKeyPoints->isChecked() == true)
		this->ui.groupBox_2->setEnabled(true);
	else
		this->ui.groupBox_2->setEnabled(false);
}

////////////////////////////////////////////////////////////////////////////
// ������pp_callback
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pp_callback(const pcl::visualization::PointPickingEvent& event, void* args)
{

	if (event.getPointIndex() == -1)
		return;

	John.clicked_points.reset(new pcl::PointCloud<pcl::PointXYZ>);

	event.getPoint(John.current_point.x, John.current_point.y, John.current_point.z);
	John.clicked_points->points.push_back(John.current_point);

	John.correctedPtr->points[John.keyOrder].x = John.current_point.x;
	John.correctedPtr->points[John.keyOrder].y = John.current_point.y;
	John.correctedPtr->points[John.keyOrder].z = John.current_point.z;

	//John.viewer->removePointCloud("selected_points");//ȥ����ѡ�еĵ���
	John.viewer->removePointCloud("cloud");
	John.showKeyPoints(John.correctedPtr, John.viewer);//ˢ��һ��
	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> red(John.clicked_points, 255, 0, 0);
	John.viewer->removePointCloud("clicked_points");
	John.viewer->addPointCloud(John.clicked_points, red, "clicked_points");
	John.viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "clicked_points");

	if (John.keyOrder == 11)
		headTopResist = 1;

	this->ui.qvtkWidget->update();
}

//////////////////////////////////////////////////////////////////////////////
//// ������pp_callback_2
//// ������
//// ���룺
//// �����Null
//// ���أ�
//// ��ע��
//// Modified by 
//////////////////////////////////////////////////////////////////////////////
//void MainWindow::pp_callback_2(const pcl::visualization::PointPickingEvent& event, void* args)
//{
//	if (event.getPointIndex() == -1)
//		return;
//
//	John.clicked_points.reset(new pcl::PointCloud<pcl::PointXYZ>);
//
//	event.getPoint(John.current_point.x, John.current_point.y, John.current_point.z);
//	John.clicked_points->points.push_back(John.current_point);
//
//	John.correctedPtr->resize(John.correctedPtr->size() + 1);
//	John.correctedPtr->points.push_back(John.current_point);
//
//	John.viewer->removePointCloud("cloud");
//	John.showKeyPoints(John.correctedPtr, John.viewer);//ˢ��һ��
//	pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> red(John.clicked_points, 255, 0, 0);
//	John.viewer->removePointCloud("clicked_points");
//	John.viewer->addPointCloud(John.clicked_points, red, "clicked_points");
//	John.viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "clicked_points");
//
//	headTopResist = 1;
//
//	this->ui.qvtkWidget->update();
//}


////////////////////////////////////////////////////////////////////////////
// ������correctKeyPoints
// ������
// ���룺
// �����
// ���أ�Null
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::correctKeyPoints()
{
	boost::mutex cloud_mutex;

	//int keyOrder = John.keyOrder;
	if (John.keyOrder == 0 || John.keyOrder == 1 || John.keyOrder == 2 || John.keyOrder == 3 || John.keyOrder == 4
		|| John.keyOrder == 5 || John.keyOrder == 6 || John.keyOrder == 7 || John.keyOrder == 8 || John.keyOrder == 9
		|| John.keyOrder == 10)
	{
		cloud_mutex.lock();
		////�����Ҫ�����ĵ�
		pcl::PointCloud<pcl::PointXYZ>::Ptr pSelect(new pcl::PointCloud<pcl::PointXYZ>);
		pSelect->points.push_back(John.correctedPtr->points.at(John.keyOrder));
		John.viewer->removePointCloud("selected_points");
		pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> halfRed(pSelect, 250, 150, 150);
		John.viewer->addPointCloud(pSelect, halfRed, "selected_points");
		John.viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "selected_points");

		////���ȡ�����
		void(MainWindow::*pfun)(const pcl::visualization::PointPickingEvent&, void*);
		pfun = &MainWindow::pp_callback;
		John.viewer->registerPointPickingCallback(pfun, *this, (void*)&John.viewer);
		cloud_mutex.unlock();
	}
	else if (John.keyOrder == 11)//���ڶ�ͷ��ߵ�
	{
		if (headTopResist == 0)
		{
			cloud_mutex.lock();
			John.viewer->removePointCloud("selected_points");
			QMessageBox::warning(NULL, QStringLiteral("��ʾ"), QStringLiteral("δʶ�𵽸û��߶�ͷ��ߵ㣡"), QMessageBox::Yes);
			void(MainWindow::*pfun)(const pcl::visualization::PointPickingEvent&, void*);
			pfun = &MainWindow::pp_callback;
			John.viewer->registerPointPickingCallback(pfun, *this, (void*)&John.viewer);
			cloud_mutex.unlock();
		}
		else if (headTopResist == 1)
		{
			cloud_mutex.lock();
			////�����Ҫ�����ĵ�
			pcl::PointCloud<pcl::PointXYZ>::Ptr pSelect(new pcl::PointCloud<pcl::PointXYZ>);
			pSelect->points.push_back(John.correctedPtr->points.at(John.keyOrder));
			John.viewer->removePointCloud("selected_points");
			pcl::visualization::PointCloudColorHandlerCustom<pcl::PointXYZ> halfRed(pSelect, 250, 150, 150);
			John.viewer->addPointCloud(pSelect, halfRed, "selected_points");
			John.viewer->setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 10, "selected_points");

			////���ȡ�����
			void(MainWindow::*pfun)(const pcl::visualization::PointPickingEvent&, void*);
			pfun = &MainWindow::pp_callback;
			John.viewer->registerPointPickingCallback(pfun, *this, (void*)&John.viewer);
			cloud_mutex.unlock();
		}
	}
}

////////////////////////////////////////////////////////////////////////////
// ������radiobutton_clicked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::radioButton_clicked()
{
	if (ui.radioButton->isChecked() == true)
	{
		John.keyOrder = 0;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_2->isChecked() == true)
	{
		John.keyOrder = 1;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_3->isChecked() == true)
	{
		John.keyOrder = 2;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_4->isChecked() == true)
	{
		John.keyOrder = 3;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_5->isChecked() == true)
	{
		
		John.keyOrder = 4;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_6->isChecked() == true)
	{
		John.keyOrder = 5;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_7->isChecked() == true)
	{
		John.keyOrder = 6;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_8->isChecked() == true)
	{
		John.keyOrder = 7;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_9->isChecked() == true)
	{
		John.keyOrder = 8;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_10->isChecked() == true)
	{
		John.keyOrder = 9;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_11->isChecked() == true)
	{
		John.keyOrder = 10;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}
	else if (ui.radioButton_12->isChecked() == true)
	{
		John.keyOrder = 11;
		correctKeyPoints();
		John.viewer->removePointCloud("clicked_points");
	}

	ui.qvtkWidget->update();
}

////////////////////////////////////////////////////////////////////////////
// ������pB_cancelRePick_clicked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_cancelRePick_clicked()
{
	ui.radioButton->setChecked(false);
	ui.radioButton_2->setChecked(false);
	ui.radioButton_3->setChecked(false);
	ui.radioButton_4->setChecked(false);
	ui.radioButton_5->setChecked(false);
	ui.radioButton_6->setChecked(false);
	ui.radioButton_7->setChecked(false);
	ui.radioButton_8->setChecked(false);
	ui.radioButton_9->setChecked(false);
	ui.radioButton_10->setChecked(false);
	ui.radioButton_11->setChecked(false);
	ui.radioButton_12->setChecked(false);
	ui.groupBox_2->setEnabled(false);
	ui.cB_changeKeyPoints->setChecked(false);

	John.viewer->removePointCloud("selected_points");
	John.viewer->removePointCloud("clicked_points");
	John.viewer->removePointCloud("cloud");
	John.showKeyPoints(John.keyPtr, John.viewer);
	ui.qvtkWidget->update();

	John.correctedPtr->clear();
	John.correctedPtr->resize(John.keyPtr->size());
	for (size_t i = 0; i < John.keyPtr->points.size(); i++) {
		John.correctedPtr->points[i].x = John.keyPtr->points[i].x;
		John.correctedPtr->points[i].y = John.keyPtr->points[i].y;
		John.correctedPtr->points[i].z = John.keyPtr->points[i].z;
	}

}

////////////////////////////////////////////////////////////////////////////
// ������pB_confirmRePick_clicked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_confirmRePick_clicked()
{
	//if (headTopResist == 1)
	//	headTopResist = 2;

	ui.radioButton->setChecked(false);
	ui.radioButton_2->setChecked(false);
	ui.radioButton_3->setChecked(false);
	ui.radioButton_4->setChecked(false);
	ui.radioButton_5->setChecked(false);
	ui.radioButton_6->setChecked(false);
	ui.radioButton_7->setChecked(false);
	ui.radioButton_8->setChecked(false);
	ui.radioButton_9->setChecked(false);
	ui.radioButton_10->setChecked(false);
	ui.radioButton_11->setChecked(false);
	ui.radioButton_12->setChecked(false);

	ui.groupBox_2->setEnabled(false);
	ui.cB_changeKeyPoints->setChecked(false);

	John.viewer->removePointCloud("selected_points");//ȥ�������ĵ�ı��
	John.viewer->removePointCloud("clicked_points");//ȥ����ѡȡ��ı��
	John.viewer->removePointCloud("cloud");//ȥ��ԭ�ؼ������
	John.showKeyPoints(John.correctedPtr, John.viewer);
	ui.qvtkWidget->update();

	string size0 = std::to_string((int)(John.computeHeadCircumference(John.cUpRight, John.correctedPtr->points.at(4))));//ͷΧ
	string size1 = std::to_string((int)(fabs(John.correctedPtr->points.at(1).y - John.correctedPtr->points.at(3).y)));//ͷ����������
	string size2 = std::to_string((int)(fabs(John.correctedPtr->points.at(7).x - John.correctedPtr->points.at(8).x)));//����
	string size3 = std::to_string((int)(fabs(John.correctedPtr->points.at(9).x - John.correctedPtr->points.at(10).x)));//�����
	string size4 = std::to_string((int)(John.computeDistance3D(John.correctedPtr->points.at(4), John.correctedPtr->points.at(0))));//ü�ĵ�����Ե����
	string size5 = std::to_string((int)(fabs(John.correctedPtr->points.at(0).z - John.correctedPtr->points.at(4).z)));//ü�ĵ�����Եˮƽ����
	string size6 = std::to_string((int)(fabs(John.correctedPtr->points.at(5).y - John.correctedPtr->points.at(6).y)));//����
	string size7 = std::to_string((int)(fabs(John.correctedPtr->points.at(0).z - John.correctedPtr->points.at(2).z)));//������ߵ㵽�Ǽ����
	string size8 = std::to_string((int)(fabs(John.correctedPtr->points.at(4).z - John.correctedPtr->points.at(2).z)));//ü�ǵ�������ߵ����

	QString s0 = QString::fromStdString(size0);
	QString s1 = QString::fromStdString(size1);
	QString s2 = QString::fromStdString(size2);
	QString s3 = QString::fromStdString(size3);
	QString s4 = QString::fromStdString(size4);
	QString s5 = QString::fromStdString(size5);
	QString s6 = QString::fromStdString(size6);
	QString s7 = QString::fromStdString(size7);
	QString s8 = QString::fromStdString(size8);

	this->ui.lineEdit_5->setText(s0);
	this->ui.lineEdit_6->setText(s1);
	this->ui.lineEdit_7->setText(s2);
	this->ui.lineEdit_9->setText(s3);
	this->ui.lineEdit_10->setText(s4);
	this->ui.lineEdit_8->setText(s5);
	this->ui.lineEdit_13->setText(s6);
	this->ui.lineEdit_11->setText(s7);
	this->ui.lineEdit_12->setText(s8);

	QString s9;
	if (headTopResist == 1)
	{
		string size9 = std::to_string((int)(fabs(John.correctedPtr->points.at(11).z - John.correctedPtr->points.at(2).z)));//������ߵ㵽��ͷ��ߵ����
		s9 = QString::fromStdString(size9);
		this->ui.lineEdit_14->setText(s9);
	}
	else
	{
		this->ui.lineEdit_14->setText(QStringLiteral("�޶�ͷ��ߵ�"));
		//s9 = "";
	}
}

////////////////////////////////////////////////////////////////////////////
// ������pB_dataRenew_clicked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_dataRenew_clicked()
{
	ui.pB_dataRenew->setEnabled(false);
	ui.pB_dataImport->setEnabled(false);
	ui.cB_changeKeyPoints->setEnabled(false);

	databaseInit();
	QSqlQuery sql_query(db);
	sql_query.exec("use head3d");

	QString name = this->ui.lineEdit_35->text();
	QString userID = this->ui.lineEdit_37->text();
	QString sex = this->ui.lineEdit_36->text();
	QString age = this->ui.lineEdit_38->text();
	QString time = this->ui.lineEdit_39->text();
	
	QString s0 = this->ui.lineEdit_5->text();
	QString s1 = this->ui.lineEdit_6->text();
	QString s2 = this->ui.lineEdit_7->text();
	QString s3 = this->ui.lineEdit_9->text();
	QString s4 = this->ui.lineEdit_10->text();
	QString s5 = this->ui.lineEdit_8->text();
	QString s6 = this->ui.lineEdit_13->text();
	QString s7 = this->ui.lineEdit_11->text();
	QString s8 = this->ui.lineEdit_12->text();
	QString s9 = this->ui.lineEdit_14->text();

	//*(John.cCorrect) += *(John.correctedPtr);//���ǵ� ���治����Ⱦ����
	if (!db.open())
	{
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("���ݿ��ʧ�ܣ�"), QMessageBox::Yes);
		return;
	}
	else
	{
		QString select_sql = "select * from patient";
		sql_query.prepare(select_sql);
		sql_query.exec(select_sql);
		while (sql_query.next())
		{
			QString mname = sql_query.value("name").toString();
			if (name == mname)
			{
				QDateTime mtime = sql_query.value("time").toDateTime();
				QString mtimeStr = mtime.toString("yyyy-MM-dd HH:mm:ss");

				if (time == mtimeStr)
				{
					QMessageBox::StandardButton rb = QMessageBox::question(NULL, QStringLiteral("����"), QStringLiteral("�������Ѵ��ڣ�ȷ�ϸ��ǣ�"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
					if (rb == QMessageBox::No)
						return;
					if (rb == QMessageBox::Yes)
					{
						QSqlQuery drop_sql(db);
						QString id = sql_query.value("id").toString();
						QString drop = QObject::tr("DELETE FROM patient WHERE id = '%1'").arg(id);
						drop_sql.prepare(drop);
						drop_sql.exec(drop);
						break;
					}
				}
			}
		}

		QString insert_sql = QObject::tr("insert into Patient values('%1','%2','%3','%4','%5','%6','%7','%8','%9','%10','%11','%12','%13','%14','%15', '%16')")
			.arg(NULL).arg(userID).arg(name).arg(sex).arg(birthStr).arg(time).arg(s0).arg(s1).arg(s2).arg(s3).arg(s4).arg(s5).arg(s6).arg(s7).arg(s8).arg(s9);
		sql_query.prepare(insert_sql);
		if (!sql_query.exec(insert_sql))
		{
			QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��������ʧ�ܣ�"), QMessageBox::Yes);
			return;
		}
		else
		{
			QMessageBox::warning(this, QStringLiteral("��ʾ"), QStringLiteral("�������ݳɹ���"), QMessageBox::Yes);

			QString _time = time.replace(":", "-").replace(" ", "-");
			std::string file(userID.toStdString() + "_" + _time.toStdString() + ".pcd");
			pcl::PCDWriter writer;
			writer.write(file, *(John.cUpRight));//��Э�������ģ��ļ��������ܳ���/��:

			pcl::PCDWriter writer_2;
			std::string file_2(userID.toStdString() + "_" + _time.toStdString() + "_KPTs.pcd");
			writer_2.write(file_2, *(John.correctedPtr));

			ui.stackedWidget->setCurrentIndex(4);
			return;
		}
	}
}

/////////////////////////////�������ݷ���ҳ��///////////////////////////////
////////////////////////////////////////////////////////////////////////////
// ������popNewWindow
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::popNewWindow()
{
	dataselect = new dataSelect(this);
	dataselect->flag = 0;
	dataselect->show();
	dataselect->setWindowTitle(QStringLiteral("����ģ��"));
	connect(dataselect, SIGNAL(selectIndex()), this, SLOT(pB_dataAnalysis_clicked()));
}

////////////////////////////////////////////////////////////////////////////
// ������popNewWindow_2
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::popNewWindow_2()
{
	dataselect = new dataSelect(this);
	dataselect->flag = 1;
	dataselect->show();
	dataselect->setWindowTitle(QStringLiteral("����ģ��"));
	connect(dataselect, SIGNAL(selectIndex_2()), this, SLOT(pB_dataAnalysis_2_clicked()));
}


////////////////////////////////////////////////////////////////////////////
// ������pB_dataAnalysis_clicked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_dataAnalysis_clicked()
{
	thread->start();//¼��������ʱ����

	this->dataselect->hide();
	//������ʾ
	progressDlg = new QProgressDialog(this);//��ʵ��һ�����Ѿ���ʼ��ʾ��������
	progressDlg->setWindowModality(Qt::WindowModal);//���θ�����
	//progressDlg->setMinimumDuration(0);
	//progressDlg->setMaximum(100);
	progressDlg->setMaximum(0);
	progressDlg->setMinimum(0);
	//progressDlg->setValue(0);
	//progressDlg->setAttribute(Qt::WA_DeleteOnClose, true);
	progressDlg->setWindowTitle(QStringLiteral("������"));
	progressDlg->setLabelText(QStringLiteral("���Ժ�ģ�͵�����Ҫ��Լ1����..."));
	progressDlg->setCancelButton(0);//����ʾȡ����ť
	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::WindowCloseButtonHint;
	progressDlg->setWindowFlags(flags);//����ʾ�����͹رհ�ť
	progressDlg->show();

	//progressDlg->setCancelButtonText(QStringLiteral("ȡ��"));
	////progressDlg->setRange(0, 100);//�����˽���������
	//progressDlg->show();
	//if (progressDlg->wasCanceled())
	//{
	//	thread->quit();
	//	thread->wait();
	//	delete workerThread_2;
	//	progressDlg->close();

	//	return;
	//}

	int curIndex = this->dataselect->curIndex;
	QString userID = this->dataselect->model->record(curIndex).value("userID").toString();
	QString name = this->dataselect->model->record(curIndex).value("name").toString();
	QString time = this->dataselect->model->record(curIndex).value("time").toDateTime().toString("yyyy-MM-dd HH:mm:ss");
	QString _time = time.replace(":", "-").replace(" ", "-");
	QString sex = this->dataselect->model->record(curIndex).value("sex").toString();
	
	QDate birth = this->dataselect->model->record(curIndex).value("birthday").toDate();
	QString birthday = birth.toString("yyyy-MM-dd");
	QDate currentDate = QDate::currentDate();
	int age = currentDate.year() - birth.year() + 1;
	QString Age = QString::number(age);
	
	QString size0 = this->dataselect->model->record(curIndex).value("size0").toString();
	QString size1 = this->dataselect->model->record(curIndex).value("size1").toString();
	QString size2 = this->dataselect->model->record(curIndex).value("size2").toString();
	QString size3 = this->dataselect->model->record(curIndex).value("size3").toString();
	QString size4 = this->dataselect->model->record(curIndex).value("size4").toString();
	QString size5 = this->dataselect->model->record(curIndex).value("size5").toString();
	QString size6 = this->dataselect->model->record(curIndex).value("size6").toString();
	QString size7 = this->dataselect->model->record(curIndex).value("size7").toString();
	QString size8 = this->dataselect->model->record(curIndex).value("size8").toString();
	QString size9 = this->dataselect->model->record(curIndex).value("size9").toString();

	//John.filename = userID.toStdString() + "_" + time.toStdString() + ".pcd";
	//John.filename_keyPoints = userID.toStdString() + "_" + time.toStdString() + "_keyPoints" + ".pcd";
	John.filename = userID.toStdString() + "_" + _time.toStdString() + ".pcd";
	John.filename_keyPoints = userID.toStdString() + "_" + _time.toStdString() + "_KPTs.pcd";

	emit ready_2();

	ui.lineEdit_29->setText(userID);
	ui.lineEdit_30->setText(name);
	ui.lineEdit_33->setText(Age);
	ui.lineEdit_34->setText(sex);
	ui.lineEdit_31->setText(this->dataselect->model->record(curIndex).value("time").toDateTime().toString("yyyy-MM-dd HH:mm:ss"));

	ui.lineEdit_21->setText(size0);
	ui.lineEdit_26->setText(size1);
	ui.lineEdit_22->setText(size2);
	ui.lineEdit_23->setText(size3);
	ui.lineEdit_19->setText(size4);
	ui.lineEdit_24->setText(size5);
	ui.lineEdit_25->setText(size6);
	ui.lineEdit_20->setText(size7);
	ui.lineEdit_27->setText(size8);
	ui.lineEdit_28->setText(size9);

	ui.groupBox_8->setEnabled(true);
	//this->dataselect->close();

	//ui.pB_exportPDF->setEnabled(true);
}

////////////////////////////////////////////////////////////////////////////
// ������pB_dataAnalysis_2_clicked
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_dataAnalysis_2_clicked()
{
	thread->start();

	int curIndex = this->dataselect->curIndex;
	QString userID = this->dataselect->model->record(curIndex).value("userID").toString();
	QString name = this->dataselect->model->record(curIndex).value("name").toString();
	//QString time = this->dataselect->model->record(curIndex).value("time").toString();
	QString time = this->dataselect->model->record(curIndex).value("time").toDateTime().toString("yyyy-MM-dd HH:mm:ss");
	QString _time = time.replace(":", "-").replace(" ", "-");
	
	QString size0 = this->dataselect->model->record(curIndex).value("size0").toString();
	QString size1 = this->dataselect->model->record(curIndex).value("size1").toString();
	QString size2 = this->dataselect->model->record(curIndex).value("size2").toString();
	QString size3 = this->dataselect->model->record(curIndex).value("size3").toString();
	QString size4 = this->dataselect->model->record(curIndex).value("size4").toString();
	QString size5 = this->dataselect->model->record(curIndex).value("size5").toString();
	QString size6 = this->dataselect->model->record(curIndex).value("size6").toString();
	QString size7 = this->dataselect->model->record(curIndex).value("size7").toString();
	QString size8 = this->dataselect->model->record(curIndex).value("size8").toString();
	QString size9 = this->dataselect->model->record(curIndex).value("size9").toString();

	//John.filename = userID.toStdString() + "_" + time.toStdString() + ".pcd";
	//John.filename_keyPoints = userID.toStdString() + "_" + time.toStdString() + "_keyPoints" + ".pcd";
	John.filename = userID.toStdString() + "_" + _time.toStdString() + ".pcd";
	John.filename_keyPoints = userID.toStdString() + "_" + _time.toStdString() + "_KPTs.pcd";
	
	if (ui.lineEdit_30->text() != name)
	{
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��ѡ��ͬһ�����ߣ�"), QMessageBox::Yes);
		return;
	}
	else
	{
		emit ready_3();

		this->dataselect->hide();

		//������ʾ
		progressDlg = new QProgressDialog(this);//��ʵ��һ�����Ѿ���ʼ��ʾ��������
		progressDlg->setWindowModality(Qt::WindowModal);//���θ�����
		//progressDlg->setMinimumDuration(0);
		//progressDlg->setMaximum(100);
		progressDlg->setMaximum(0);
		progressDlg->setMinimum(0);
		//progressDlg->setValue(0);
		//progressDlg->setAttribute(Qt::WA_DeleteOnClose, true);
		progressDlg->setWindowTitle(QStringLiteral("������"));
		progressDlg->setLabelText(QStringLiteral("���Ժ�ģ�͵�����Ҫ��Լ1����..."));
		progressDlg->setCancelButton(0);
		Qt::WindowFlags flags = Qt::Dialog;
		flags |= Qt::WindowCloseButtonHint;
		progressDlg->setWindowFlags(flags);
		progressDlg->show();
		//progressDlg->setCancelButtonText(QStringLiteral("ȡ��"));
		////progressDlg->setRange(0, 100);//�����˽���������
		//progressDlg->show();
		//if (progressDlg->wasCanceled())
		//{
		//	thread->quit();
		//	thread->wait();
		//	delete workerThread_3;
		//	progressDlg->close();

		//	return;
		//}

		//ui.lineEdit_29->setText(id);
		ui.lineEdit_32->setText(this->dataselect->model->record(curIndex).value("time").toDateTime().toString("yyyy-MM-dd HH:mm:ss"));

		ui.lineEdit_53->setText(size0);
		ui.lineEdit_51->setText(size1);
		ui.lineEdit_58->setText(size2);
		ui.lineEdit_52->setText(size3);
		ui.lineEdit_56->setText(size4);
		ui.lineEdit_49->setText(size5);
		ui.lineEdit_57->setText(size6);
		ui.lineEdit_54->setText(size7);
		ui.lineEdit_55->setText(size8);
		ui.lineEdit_50->setText(size9);

		ui.groupBox_8->setEnabled(true);

		int d0 = ui.lineEdit_21->text().toInt() - ui.lineEdit_53->text().toInt();
		int d1 = ui.lineEdit_26->text().toInt() - ui.lineEdit_51->text().toInt();
		int d2 = ui.lineEdit_22->text().toInt() - ui.lineEdit_58->text().toInt();
		int d3 = ui.lineEdit_23->text().toInt() - ui.lineEdit_52->text().toInt();
		int d4 = ui.lineEdit_19->text().toInt() - ui.lineEdit_56->text().toInt();
		int d5 = ui.lineEdit_24->text().toInt() - ui.lineEdit_49->text().toInt();
		int d6 = ui.lineEdit_25->text().toInt() - ui.lineEdit_57->text().toInt();
		int d7 = ui.lineEdit_20->text().toInt() - ui.lineEdit_54->text().toInt();
		int d8 = ui.lineEdit_27->text().toInt() - ui.lineEdit_55->text().toInt();
		int d9 = ui.lineEdit_28->text().toInt() - ui.lineEdit_50->text().toInt();

		ui.lineEdit_59->setText(QString::number(d0));
		ui.lineEdit_63->setText(QString::number(d1));
		ui.lineEdit_65->setText(QString::number(d2));
		ui.lineEdit_64->setText(QString::number(d3));
		ui.lineEdit_68->setText(QString::number(d4));
		ui.lineEdit_66->setText(QString::number(d5));
		ui.lineEdit_61->setText(QString::number(d6));
		ui.lineEdit_60->setText(QString::number(d7));
		ui.lineEdit_62->setText(QString::number(d8));
		ui.lineEdit_67->setText(QString::number(d9));
	}	
}

////////////////////////////////////////////////////////////////////////////
// ������finishInput
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::finishInput()
{
	this->dataselect->close();
	progressDlg->close();

	//John.viewer_2.reset(new pcl::visualization::PCLVisualizer("viewer_2", false));
	//John.viewer_2->initCameraParameters();

	//int vleft(0);
	//John.viewer_2->createViewPort(0.0, 0.0, 0.5, 1.0, vleft);
	//John.viewer_2->setBackgroundColor(0.7, 0.7, 0.7, vleft);
	//John.viewer_2->setCameraPosition(0, 0, 500, 0, 1, 0);

	//int vright(1);
	//John.viewer_2->createViewPort(0.5, 0.0, 1.0, 1.0, vright);
	//John.viewer_2->setBackgroundColor(0.8, 0.8, 0.8, vright);
	//John.viewer_2->createViewPortCamera(vright);
	//John.viewer_2->setCameraPosition(800, 0, 0, 0, 1, 0, vright);
	//John.viewer_2->addCoordinateSystem(20.0, "Coordinate_2");

	////John.viewer->addPointCloud(John.cloud, "cloud");
	//ui.qvtkWidget_2->SetRenderWindow(John.viewer_2->getRenderWindow());
	//John.viewer_2->setupInteractor(ui.qvtkWidget_2->GetInteractor(), ui.qvtkWidget_2->GetRenderWindow());
	//ui.qvtkWidget_2->update();

	John.showModel(John.mesh, John.viewer_2);
	ui.qvtkWidget_2->update();
	John.showKeyPoints(John.cloud, John.viewer_2);
	ui.qvtkWidget_2->update();

	thread->quit();
	thread->wait();
	//delete workerThread_2;
}

////////////////////////////////////////////////////////////////////////////
// ������finishInput_2
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::finishInput_2()
{
	this->dataselect->close();
	progressDlg->close();


	//John.viewer_3.reset(new pcl::visualization::PCLVisualizer("viewer_3", false));
	//John.viewer_3->initCameraParameters();
	//int vleft(0);
	//John.viewer_3->createViewPort(0.0, 0.0, 0.5, 1.0, vleft);
	//John.viewer_3->setBackgroundColor(0.7, 0.7, 0.7, vleft);
	//John.viewer_3->setCameraPosition(0, 0, 500, 0, 1, 0);
	//int vright(1);
	//John.viewer_3->createViewPort(0.5, 0.0, 1.0, 1.0, vright);
	//John.viewer_3->setBackgroundColor(0.8, 0.8, 0.8, vright);
	//John.viewer_3->createViewPortCamera(vright);
	//John.viewer_3->setCameraPosition(800, 0, 0, 0, 1, 0, vright);
	//John.viewer_3->addCoordinateSystem(20.0, "Coordinate_3");
	//ui.qvtkWidget_3->SetRenderWindow(John.viewer_3->getRenderWindow());
	//John.viewer_3->setupInteractor(ui.qvtkWidget_3->GetInteractor(), ui.qvtkWidget_3->GetRenderWindow());
	//ui.qvtkWidget_3->update();

	John.showModel(John.mesh, John.viewer_3);
	ui.qvtkWidget_3->update();
	John.showKeyPoints(John.cloud, John.viewer_3);
	ui.qvtkWidget_3->update();

	thread->quit();
	thread->wait();
	//delete workerThread_3;
}

////////////////////////////////////////////////////////////////////////////
// ������pB_exportPDF_clicked
// ����������PDF����
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void MainWindow::pB_exportPDF_clicked()
{
	//system("pause");//��ͣ���ƴ���ˢ���Խ�ȡͼƬ
	QString path = QFileDialog::getSaveFileName(this, tr("Open File"), ".", tr("PDF Files(*.pdf)"));//����QFileDialog�Ի���ѡ���ļ��洢·��  
	if (!path.isEmpty())
	{
		//���û��д��׺���Զ�����  
		if (QFileInfo(path).suffix().isEmpty())
			path.append(".pdf");
		//ʵ�������ļ�������ֻд�ķ�ʽ��  
		QFile pdfFile(path);
		if (!pdfFile.open(QIODevice::WriteOnly))
		{
			QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��ѡ����ȷ·����"));
			return;
		}
		QPdfWriter *pdfWriter = new QPdfWriter(&pdfFile);//ʵ����QPdfWriter  
		pdfWriter->setPageSize(QPagedPaintDevice::A4);//����ֽ��ΪA4ֽ  
		//pdfWriter->setResolution(QPrinter::ScreenResolution);//���÷ֱ���  
		pdfWriter->setResolution(300);//���÷ֱ��� ����3508*2479
		pdfWriter->setTitle("�Ա�֢������άͷ����Ϸ�������");
		QPainter *pdfPainter = new QPainter(pdfWriter);//ʵ����QPainter  

		int pageMargin = 100;
		pdfWriter->setPageMargins(QMarginsF(pageMargin, pageMargin, pageMargin, pageMargin));

		QTextOption option(Qt::AlignHCenter | Qt::AlignVCenter);//ˮƽ����ֱ����
		option.setWrapMode(QTextOption::WordWrap);//�Զ�����
		int yCurrentP = 0;
		int xCurrentP = 0;
		int contentWidth = 2479 - pageMargin;

		QFont font;
		font.family();
		font.setFamily("Times New Roman");
		int fontSize = 22;
		int textHeight = 100;
		font.setPointSize(fontSize);
		font.setBold(true);
		pdfPainter->setFont(font);

		yCurrentP += 60;
		option.setAlignment(Qt::AlignCenter);
		pdfPainter->drawText(QRect(0, yCurrentP, contentWidth, textHeight), QStringLiteral("�Ա�֢������άͷ����Ϸ�������"), option);//����֧��

		yCurrentP += (textHeight + 60);
		fontSize = 10;
		font.setPointSize(fontSize);
		pdfPainter->setFont(font);
		textHeight = 40;
		option.setAlignment(Qt::AlignLeft);
		pdfPainter->drawText(QRect((xCurrentP + 1800), yCurrentP, contentWidth, textHeight), QStringLiteral("�����¼�ߣ�") + admin, option);

		QDateTime currentTime = QDateTime::currentDateTime();
		QString timeString = QStringLiteral("����ʱ�䣺") + currentTime.toString("yyyy-MM-dd HH:mm");
		yCurrentP += (textHeight + 10);
		pdfPainter->drawText(QRect((xCurrentP + 1800), yCurrentP, contentWidth, textHeight), timeString, option);

		QPen pen;
		yCurrentP += (textHeight + 20);
		int penHeigth = 8;
		pen.setWidth(penHeigth);
		pen.setColor(Qt::red);
		pdfPainter->setPen(pen);
		pdfPainter->drawLine(xCurrentP, yCurrentP, 2379, yCurrentP);

		yCurrentP += (textHeight + 20);
		fontSize = 16;
		font.setPointSize(fontSize);
		pdfPainter->setFont(font);
		textHeight = 80;
		option.setAlignment(Qt::AlignLeft);
		pen.setColor(Qt::black);
		pdfPainter->setPen(pen);
		pdfPainter->drawText(QRect(xCurrentP, yCurrentP, contentWidth, textHeight), QStringLiteral("������Ϣ"), option);

		fontSize = 12;
		font.setPointSize(fontSize);
		font.setBold(false);
		pdfPainter->setFont(font);
		yCurrentP += (textHeight + 120);
		pdfPainter->drawText(QRect(140, yCurrentP, contentWidth, textHeight), QStringLiteral("����������") + ui.lineEdit_30->text(), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 100), contentWidth, textHeight), QStringLiteral("���߱�ţ�") + ui.lineEdit_29->text(), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 200), contentWidth, textHeight), QStringLiteral("�����Ա�") + ui.lineEdit_34->text(), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 300), contentWidth, textHeight), QStringLiteral("�������䣺") + ui.lineEdit_33->text(), option);

		yCurrentP += (textHeight + 360);
		fontSize = 16;
		font.setPointSize(fontSize);
		font.setBold(true);
		pdfPainter->setFont(font);
		contentWidth = 800;
		pdfPainter->drawText(QRect(xCurrentP, yCurrentP, contentWidth, textHeight), QStringLiteral("ǰ��ͷ��ģ��"), option);
		pdfPainter->drawText(QRect((xCurrentP + 1250), yCurrentP, contentWidth, textHeight), QStringLiteral("����ͷ��ģ��"), option);
		yCurrentP += (textHeight + 20);
		pdfPainter->drawText(QRect(xCurrentP, yCurrentP, contentWidth, textHeight), ui.lineEdit_31->text(), option);
		pdfPainter->drawText(QRect((xCurrentP + 1250), yCurrentP, contentWidth, textHeight), ui.lineEdit_32->text(), option);

		yCurrentP += (textHeight + 20);
		QScreen *screen = QGuiApplication::primaryScreen();
		screen->grabWindow(this->ui.stackedWidget->winId()).save("result.png");
		//QPixmap pixmap1 = QPixmap::grabWidget(ui.qvtkWidget_2);
		//QPixmap pixmap1 = QPixmap::grabWindow(this->viewer_2);
		//pixmap1.save("result_1.bmp");


		std::string imageFileName = "result.png";
		QPixmap roxImage = QPixmap(QString::fromStdString(imageFileName));

		QPixmap result_1 = roxImage.copy(ui.qvtkWidget_2->x() + 10, ui.qvtkWidget_2->y() + 10, ui.qvtkWidget_2->width() - 10, ui.qvtkWidget_2->height() - 10);
		QPixmap result_2 = roxImage.copy(ui.qvtkWidget_3->x() + 10, ui.qvtkWidget_3->y() + 10, ui.qvtkWidget_3->width() - 10, ui.qvtkWidget_3->height() - 10);//ע���ȡ�߽�
		pdfPainter->drawPixmap(xCurrentP, yCurrentP, 1100, 600, result_1);
		pdfPainter->drawPixmap((xCurrentP + 1250), yCurrentP, 1100, 600, result_2);

		QFile::remove("result.png");

		yCurrentP += 940;
		pdfPainter->drawText(QRect(xCurrentP, yCurrentP, contentWidth, textHeight), QStringLiteral("�ؼ��ߴ����ݷ���"), option);

		//�ֹ����Ʊ��
		yCurrentP += 200;
		fontSize = 12;
		font.setPointSize(fontSize);
		font.setBold(false);
		pdfPainter->setFont(font);
		option.setAlignment(Qt::AlignLeft);
		pdfPainter->drawText(QRect(140, yCurrentP, contentWidth, textHeight), QStringLiteral(""), option);
		pdfPainter->drawText(QRect(940, yCurrentP, contentWidth, textHeight), QStringLiteral("ǰ������(mm)"), option);
		pdfPainter->drawText(QRect(1340, yCurrentP, contentWidth, textHeight), QStringLiteral("��������(mm)"), option);
		pdfPainter->drawText(QRect(1740, yCurrentP, contentWidth, textHeight), QStringLiteral("��������(mm)"), option);

		pdfPainter->drawText(QRect(140, (yCurrentP + 80), contentWidth, textHeight), QStringLiteral("ͷΧ"), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 160), contentWidth, textHeight), QStringLiteral("ͷ������򦴹ֱ����"), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 240), contentWidth, textHeight), QStringLiteral("���տ��"), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 320), contentWidth, textHeight), QStringLiteral("�����"), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 400), contentWidth, textHeight), QStringLiteral("ü�����Ǽ����"), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 480), contentWidth, textHeight), QStringLiteral("ü�����Ǽ�ˮƽ����"), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 560), contentWidth, textHeight), QStringLiteral("����"), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 640), contentWidth, textHeight), QStringLiteral("������ߵ����Ǽ�ˮƽ����"), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 720), contentWidth, textHeight), QStringLiteral("������ߵ���ü��ˮƽ����"), option);
		pdfPainter->drawText(QRect(140, (yCurrentP + 800), contentWidth, textHeight), QStringLiteral("������ߵ�����ͷ��ߵ����"), option);

		//ǰ������
		pdfPainter->drawText(QRect(940, (yCurrentP + 80), contentWidth, textHeight), ui.lineEdit_21->text(), option);
		pdfPainter->drawText(QRect(940, (yCurrentP + 160), contentWidth, textHeight), ui.lineEdit_26->text(), option);
		pdfPainter->drawText(QRect(940, (yCurrentP + 240), contentWidth, textHeight), ui.lineEdit_22->text(), option);
		pdfPainter->drawText(QRect(940, (yCurrentP + 320), contentWidth, textHeight), ui.lineEdit_23->text(), option);
		pdfPainter->drawText(QRect(940, (yCurrentP + 400), contentWidth, textHeight), ui.lineEdit_19->text(), option);
		pdfPainter->drawText(QRect(940, (yCurrentP + 480), contentWidth, textHeight), ui.lineEdit_24->text(), option);
		pdfPainter->drawText(QRect(940, (yCurrentP + 560), contentWidth, textHeight), ui.lineEdit_25->text(), option);
		pdfPainter->drawText(QRect(940, (yCurrentP + 640), contentWidth, textHeight), ui.lineEdit_20->text(), option);
		pdfPainter->drawText(QRect(940, (yCurrentP + 720), contentWidth, textHeight), ui.lineEdit_27->text(), option);
		pdfPainter->drawText(QRect(940, (yCurrentP + 800), contentWidth, textHeight), ui.lineEdit_28->text(), option);

		//��������
		pdfPainter->drawText(QRect(1340, (yCurrentP + 80), contentWidth, textHeight), ui.lineEdit_53->text(), option);
		pdfPainter->drawText(QRect(1340, (yCurrentP + 160), contentWidth, textHeight), ui.lineEdit_51->text(), option);
		pdfPainter->drawText(QRect(1340, (yCurrentP + 240), contentWidth, textHeight), ui.lineEdit_58->text(), option);
		pdfPainter->drawText(QRect(1340, (yCurrentP + 320), contentWidth, textHeight), ui.lineEdit_52->text(), option);
		pdfPainter->drawText(QRect(1340, (yCurrentP + 400), contentWidth, textHeight), ui.lineEdit_56->text(), option);
		pdfPainter->drawText(QRect(1340, (yCurrentP + 480), contentWidth, textHeight), ui.lineEdit_49->text(), option);
		pdfPainter->drawText(QRect(1340, (yCurrentP + 560), contentWidth, textHeight), ui.lineEdit_57->text(), option);
		pdfPainter->drawText(QRect(1340, (yCurrentP + 640), contentWidth, textHeight), ui.lineEdit_54->text(), option);
		pdfPainter->drawText(QRect(1340, (yCurrentP + 720), contentWidth, textHeight), ui.lineEdit_55->text(), option);
		pdfPainter->drawText(QRect(1340, (yCurrentP + 800), contentWidth, textHeight), ui.lineEdit_50->text(), option);

		//���ݱ仯
		pdfPainter->drawText(QRect(1740, (yCurrentP + 80), contentWidth, textHeight), ui.lineEdit_59->text(), option);
		pdfPainter->drawText(QRect(1740, (yCurrentP + 160), contentWidth, textHeight), ui.lineEdit_63->text(), option);
		pdfPainter->drawText(QRect(1740, (yCurrentP + 240), contentWidth, textHeight), ui.lineEdit_65->text(), option);
		pdfPainter->drawText(QRect(1740, (yCurrentP + 320), contentWidth, textHeight), ui.lineEdit_64->text(), option);
		pdfPainter->drawText(QRect(1740, (yCurrentP + 400), contentWidth, textHeight), ui.lineEdit_68->text(), option);
		pdfPainter->drawText(QRect(1740, (yCurrentP + 480), contentWidth, textHeight), ui.lineEdit_66->text(), option);
		pdfPainter->drawText(QRect(1740, (yCurrentP + 560), contentWidth, textHeight), ui.lineEdit_61->text(), option);
		pdfPainter->drawText(QRect(1740, (yCurrentP + 640), contentWidth, textHeight), ui.lineEdit_60->text(), option);
		pdfPainter->drawText(QRect(1740, (yCurrentP + 720), contentWidth, textHeight), ui.lineEdit_62->text(), option);
		pdfPainter->drawText(QRect(1740, (yCurrentP + 800), contentWidth, textHeight), ui.lineEdit_67->text(), option);

		yCurrentP = 3300;
		pen.setWidth(8);
		pen.setColor(Qt::red);
		pdfPainter->setPen(pen);
		pdfPainter->drawLine(0, yCurrentP, 2379, yCurrentP);

		delete pdfPainter;
		delete pdfWriter;
		pdfFile.close();

		//��PDF
		QDesktopServices::openUrl(QUrl::fromLocalFile(path));
	}

	else
		QMessageBox::warning(this, QStringLiteral("����"), QStringLiteral("��ѡ�񱣴�·����"), QMessageBox::Yes);
}



