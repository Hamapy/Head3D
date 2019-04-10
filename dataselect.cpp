#pragma warning(disable:4819)
#include "dataselect.h"

//QSqlDatabase db;

dataSelect::dataSelect(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	db = QSqlDatabase::addDatabase("QMYSQL");
	//��ȡmysql���������ļ�
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

dataSelect::~dataSelect()
{

}

////////////////////////////////////////////////////////////////////////////
// ������dataSelectInit
// ���������ݿ����ӳ�ʼ��
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect::dataSelectInit()
{
	QSqlQuery sql_query(db);
	if (!db.open())
	{
		QMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���ݿ��ʧ�ܣ�"), QMessageBox::Yes);
		return;
	}
	else
	{
		sql_query.exec("use head3d");
		//����������Ŀ
		QStringList strings;
		QCompleter *completer;
		sql_query.exec("SELECT name FROM patient");
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
		ui.comboBox->clear();
		ui.comboBox->addItems(strings);
		ui.comboBox->setCompleter(completer);
		ui.comboBox->setCurrentText("");

		QStringList strings_2;
		sql_query.exec("SELECT userid FROM patient");
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
		model->setTable("Patient");
		model->setSort(1, Qt::AscendingOrder);
		model->setHeaderData(0, Qt::Horizontal, QStringLiteral("���"));
		model->setHeaderData(1, Qt::Horizontal, QStringLiteral("���߱��"));
		model->setHeaderData(2, Qt::Horizontal, QStringLiteral("��������"));
		model->setHeaderData(3, Qt::Horizontal, QStringLiteral("�����Ա�"));
		model->setHeaderData(4, Qt::Horizontal, QStringLiteral("���߳�������"));
		model->setHeaderData(5, Qt::Horizontal, QStringLiteral("����ʱ��"));
		model->setHeaderData(6, Qt::Horizontal, QStringLiteral("ͷΧ"));
		model->setHeaderData(7, Qt::Horizontal, QStringLiteral("ͷ������򦴹ֱ����"));
		model->setHeaderData(8, Qt::Horizontal, QStringLiteral("���տ��"));
		model->setHeaderData(9, Qt::Horizontal, QStringLiteral("�����"));
		model->setHeaderData(10, Qt::Horizontal, QStringLiteral("ü�����Ǽ����"));
		model->setHeaderData(11, Qt::Horizontal, QStringLiteral("ü�����Ǽ�ˮƽ����"));
		model->setHeaderData(12, Qt::Horizontal, QStringLiteral("����"));
		model->setHeaderData(13, Qt::Horizontal, QStringLiteral("������ߵ����Ǽ�ˮƽ����"));
		model->setHeaderData(14, Qt::Horizontal, QStringLiteral("������ߵ���ü��ˮƽ����"));
		model->setHeaderData(15, Qt::Horizontal, QStringLiteral("������ߵ�����ͷ��ߵ�ˮƽ����"));

		ui.tableView->setModel(model);
		ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
		ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
		ui.tableView->setColumnHidden(0, true);//���ص�һ��
		ui.tableView->resizeColumnsToContents();
		ui.tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

		QHeaderView *header = ui.tableView->horizontalHeader();
		header->setStretchLastSection(true);//���ó�������
		header->setSectionResizeMode(QHeaderView::ResizeToContents);//�п���Ӧ����
		ui.tableView->show();
		model->select(); //��ѯ���ű�
	}
}

////////////////////////////////////////////////////////////////////////////
// ������query
// ���������ݿ��ѯ
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect::query()
{
	QString select;
	select = "1=1";//������û��ʵ�����壬��֤��������еġ�and����Ч
	if (ui.radioButton->isChecked())
	{
		select = QString("name = '%1'").arg(ui.comboBox->currentText());
		model->setFilter(select);
		model->select();//������Ϣ��ͬʱ������ͼ
		select = "";
	}
	else if (ui.radioButton_2->isChecked())
	{
		select = QString("userID = '%1'").arg(ui.comboBox_2->currentText());
		model->setFilter(select);
		model->select();//������Ϣ��ͬʱ������ͼ
		select = "";
	}
	else if (ui.radioButton_3->isChecked())
	{
		select = QString("birthday >= '%1' and birthday <= '%2'").arg(ui.dateEdit->date().toString("yyyy-MM-dd")).arg(ui.dateEdit_2->date().toString("yyyy-MM-dd"));
		model->setFilter(select);
		model->select();//������Ϣ��ͬʱ������ͼ
		select = "";
	}
	else if (ui.radioButton_4->isChecked())
	{
		select = QString("time >= '%1' and time <= '%2'").arg(ui.dateEdit_3->date().toString("yyyy-MM-dd")).arg(ui.dateEdit_4->date().toString("yyyy-MM-dd"));
		model->setFilter(select);
		model->select();//������Ϣ��ͬʱ������ͼ
		select = "";
	}
}

////////////////////////////////////////////////////////////////////////////
// ������reset
// ������������������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect::reset()
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
// ������comoBoxEditable
// ������
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect::comoBoxEditable()
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
// ������confirm
// ������ȷ�����ݿ���Ŀѡ��
// ���룺
// �����Null
// ���أ�
// ��ע��
// Modified by 
////////////////////////////////////////////////////////////////////////////
void dataSelect::confirm()
{
	curIndex = ui.tableView->currentIndex().row();//��ȡѡ�е���
	if (flag == 0)
		emit selectIndex();
	else if (flag == 1)
		emit selectIndex_2();
	else if (flag == 2)
	{
		//QMessageBox::StandardButton rb = QMessageBox::question(NULL, QStringLiteral("����"), QStringLiteral("ȷ��ѡ��û��ߣ�"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
		//if (rb == QMessageBox::No)
		//	return;
		//if (rb == QMessageBox::Yes)
		//{
			emit selectPatient();
		//}
	}
}
