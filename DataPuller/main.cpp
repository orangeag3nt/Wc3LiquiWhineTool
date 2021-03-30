#include "RecordLoader.h"
#include "CsvWriter.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	RecordLoader l;
	QObject::connect(&l, &RecordLoader::signalFinished, &app, &QCoreApplication::quit);
	//l.start("1.32.9", QDate(2020, 10, 21), QDateTime::currentDateTime().date());
	l.start("1.32.9", QDate(2021, 02, 03), QDate(2021, 02, 12));
	int res = app.exec();

	QFile f("e:/export.csv");
	if (!f.open(QIODevice::WriteOnly)) {
		qDebug() << f.errorString();
		return 666;
	}
	QTextStream stream(&f);
	stream.setCodec("UTF-8");
	stream.setGenerateByteOrderMark(true);

	CsvWriter writer(&stream);
	writer.write(QStringList() << "Map" << "Winner" << "Winner race" << "Loser" << "Loser race");
	for (const Record& rec : l.result()) {
		writer.write(QStringList() << rec.map << rec.winner << rec.winnerRace << rec.loser << rec.loserRace);
	}
	qDebug() << "Export completed.";
	return res;
}