#pragma warning(disable:4819)
#include "mainWindow.h"
#include <QtWidgets/QApplication>


int main(int argc, char *argv[])
{
	//QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));
	//QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
	//QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	QApplication a(argc, argv);
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF8"));

	//WorkerThread *workerThread = new WorkerThread();
	//QThread *thread = new QThread();
	//workerThread->moveToThread(thread);
	//thread->start();

	MainWindow welcome;
	welcome.show();
	return a.exec();
}
