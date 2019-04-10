#pragma warning(disable:4819)
#include "dataselect_2.h"

//QSqlDatabase db;

dataSelect_2::dataSelect_2(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	db = QSqlDatabase::addDatabase("QMYSQL");
	//读取mysql连接配置文件
	QSettings *m_IniFile = new QSettings("MyHead3D.ini", QSettings::IniFormat);
	QString hostName = m_IniFile->value("/mysql-connect/HostName").toString();
	QString databaseName = m_IniFile->value("/mysql-connect/DatabaseName").toString();
	QString userName = m_IniFile->value("/mysql-connect/UserName").toString();
	QString password = m_IniFile->value("/mysql-connect/Password").toString();

	db.setHostName(hostName);
	db.setDatabaseName(databaseName);
	db.setUserName(userName);
	db.setPassword(password);

	dataSelectInit();
	connect(this->ui.radioButton, SIGNAL(clicked()), this, SLOT(comoBoxEditable()));
	connect(this->ui.radioButton_2, SIGNAL(clicked()), this, SLOT(comoBoxEditable()));
	connect(this->ui.radioButton_3, SIGNAL(clicked()), this, SLOT(comoBoxEditable()));
	connect(this->ui.radioButton_4, SIGNAL(clicked()), this, SLOT(comoBoxEditable()));
	connect(this->ui.pushButton, SIGNAL(pressed()), this, SLOT(query()));
	connect(this->ui.pushButton_2, SIGNAL(pressed()), this, SLOT(confirm()));
	connect(this->ui.pushButton_3, SIGNAL(pressed()), this, SLOT(reset()));
}

dataSelect_2::~dataSelect_2()
{

}

////////////////////////////////////////////////////////////////////////////
// 函数：query
// 描述：数据库连接初始化
// 输入：
// 输出：Null
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect_2::dataSelectInit()
{
	QSqlQuery sql_query(db);
	if (!db.open())
	{
		QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("数据库打开失败！"), QMessageBox::Yes);
		return;
	}
	else
	{
		sql_query.exec("use head3d");
		//设置下拉条目
		QStringList strings;
		QCompleter *completer;
		sql_query.exec("SELECT name FROM patientInformation");
		while (sql_query.next()) {
			QString name = sql_query.value("name").toString();
			strings.append(name);
		}
		//去重
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
		ui.comboBox->clear();
		ui.comboBox->addItems(strings);
		ui.comboBox->setCompleter(completer);
		ui.comboBox->setCurrentText("");

		QStringList strings_2;
		sql_query.exec("SELECT userid FROM patientInformation");
		while (sql_query.next()) {
			QString userID = sql_query.value("userID").toString();
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
		ui.comboBox_2->clear();
		ui.comboBox_2->addItems(strings_2);
		ui.comboBox_2->setCompleter(completer);
		ui.comboBox_2->setCurrentText("");

		ui.comboBox->setEnabled(false);
		ui.comboBox_2->setEnabled(false);
		ui.dateEdit->setEnabled(false);
		ui.dateEdit_2->setEnabled(false);
		ui.dateEdit_3->setEnabled(false);
		ui.dateEdit_4->setEnabled(false);

		model = new QSqlTableModel(this);
		model->setTable("PatientInformation");
		model->setSort(1, Qt::AscendingOrder);
		//model->setHeaderData(0, Qt::Horizontal, QStringLiteral("序号"));
		model->setHeaderData(1, Qt::Horizontal, QStringLiteral("患者编号"));
		model->setHeaderData(2, Qt::Horizontal, QStringLiteral("患者姓名"));
		model->setHeaderData(3, Qt::Horizontal, QStringLiteral("患者性别"));
		model->setHeaderData(4, Qt::Horizontal, QStringLiteral("患者出生日期"));
		model->setHeaderData(5, Qt::Horizontal, QStringLiteral("录入时间"));

		ui.tableView->setModel(model);
		ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView->setColumnHidden(0, true);//隐藏第一列

		ui.tableView->resizeColumnsToContents();
		ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

		QHeaderView *header = ui.tableView->horizontalHeader();
		header->setStretchLastSection(true);//设置充满表宽度
		header->setSectionResizeMode(QHeaderView::ResizeToContents);
		//header->setSectionResizeMode(QHeaderView::Stretch);//均匀列宽
		ui.tableView->show();
		model->select(); //查询整张表
	}
}

////////////////////////////////////////////////////////////////////////////
// 函数：query
// 描述：数据库查询
// 输入：
// 输出：Null
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect_2::query()
{
	QString select;
	select = "1=1";//此设置没有实际意义，保证过滤语句中的“and”有效
	if (ui.radioButton->isChecked())
	{
		select = QString("name = '%1'").arg(ui.comboBox->currentText());
		model->setFilter(select);
		model->select();//过滤信息，同时更新视图
		select = "";
	}
	else if (ui.radioButton_2->isChecked())
	{
		select = QString("userID = '%1'").arg(ui.comboBox_2->currentText());
		model->setFilter(select);
		model->select();//过滤信息，同时更新视图
		select = "";
	}
	else if (ui.radioButton_3->isChecked())
	{
		select = QString("birthday >= '%1' and birthday <= '%2'").arg(ui.dateEdit->date().toString("yyyy-MM-dd")).arg(ui.dateEdit_2->date().toString("yyyy-MM-dd"));
		model->setFilter(select);
		model->select();//过滤信息，同时更新视图
		select = "";
	}
	else if (ui.radioButton_4->isChecked())
	{
		select = QString("registertime >= '%1' and registertime <= '%2'").arg(ui.dateEdit_3->date().toString("yyyy-MM-dd")).arg(ui.dateEdit_4->date().toString("yyyy-MM-dd"));
		model->setFilter(select);
		model->select();//过滤信息，同时更新视图
		select = "";
	}
	
}

////////////////////////////////////////////////////////////////////////////
// 函数：reset
// 描述：重置搜索条件
// 输入：
// 输出：Null
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect_2::reset()
{
	ui.radioButton->setChecked(false);
	ui.radioButton_2->setChecked(false);
	ui.radioButton_3->setChecked(false);
	ui.radioButton_4->setChecked(false);
	ui.comboBox->setCurrentText("");
	ui.comboBox_2->setCurrentText("");
	ui.comboBox->setEnabled(false);
	ui.comboBox_2->setEnabled(false);
	ui.dateEdit->setEnabled(false);
	ui.dateEdit_2->setEnabled(false);
	ui.dateEdit_3->setEnabled(false);
	ui.dateEdit_4->setEnabled(false);
	//ui.comboBox_3->setCurrentText("");
	//ui.comboBox_4->setCurrentText("");

	QString select = "1=1";
	model->setFilter(select);
	model->select();
}

////////////////////////////////////////////////////////////////////////////
// 函数：comoBoxEditable
// 描述：
// 输入：
// 输出：Null
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect_2::comoBoxEditable()
{
	QTime reachTime = QTime::currentTime().addMSecs(100);
	while (QTime::currentTime() < reachTime)
	{
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}
	if (ui.radioButton->isChecked() == true)
	{
		ui.comboBox->setEnabled(true);
	}
	else
	{
		ui.comboBox->setEnabled(false);
	}


	if (ui.radioButton_2->isChecked() == true)
	{
		ui.comboBox_2->setEnabled(true);
	}
	else
	{
		ui.comboBox_2->setEnabled(false);
	}


	if (ui.radioButton_3->isChecked() == true)
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

	if (ui.radioButton_4->isChecked() == true)
	{
		ui.dateEdit_3->setEnabled(true);
		ui.dateEdit_4->setEnabled(true);
		ui.dateEdit_3->setCalendarPopup(true);
		ui.dateEdit_4->setCalendarPopup(true);
	}
	else
	{
		ui.dateEdit_3->setEnabled(false);
		ui.dateEdit_4->setEnabled(false);
	}
}

////////////////////////////////////////////////////////////////////////////
// 函数：confirm
// 描述：确认数据库条目选择
// 输入：
// 输出：Null
// 返回：
// 备注：
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect_2::confirm()
{
	curIndex = ui.tableView->currentIndex().row();//获取选中的行
	if (flag == 0)
		emit selectIndex();
	else if (flag == 1)
		emit selectIndex_2();
	else if (flag == 2)
	{
		//QMessageBox::StandardButton rb = QMessageBox::question(NULL, QStringLiteral("提醒"), QStringLiteral("确认选择该患者？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		//if (rb == QMessageBox::No)
		//	return;
		//if (rb == QMessageBox::Yes)
		//{
			emit selectPatient();
		//}
	}
}
