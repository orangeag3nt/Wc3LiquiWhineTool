#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include "RecordLoader.h"

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	RecordLoader l;
	QObject::connect(&l, &RecordLoader::signalFinished, &app, &QCoreApplication::quit);
	l.start("1.32.9");
	int res = app.exec();
	//for (const Record& rec : l.result())
		qDebug() << l.result().count();
	return res;
}