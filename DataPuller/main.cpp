#include "RecordLoader.h"
#include "CsvWriter.h"
#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QFile>

int main(int argc, char *argv[])
{
	QCoreApplication app(argc, argv);
	RecordLoader l;
	QObject::connect(&l, &RecordLoader::signalFinished, &app, &QCoreApplication::quit);
	//l.start("1.32.9", QDate(2020, 10, 21), QDateTime::currentDateTime().date());
	l.start("1.32.9", QDate(2020, 10, 21), QDate(2020, 10, 22));
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
	writer.write(QStringList() << "Date" << "Match name" << "Map" << "Winner" << "Winner race" << "Loser" << "Loser race");
	QList<Record> dataset = l.result();
	std::sort(dataset.begin(), dataset.end(), [](const Record& r1, const Record& r2)->bool {
		if (r1.date == r2.date)
			return QString::compare(r1.name, r2.name) < 0;
		else
			return r1.date < r2.date;
	});
	for (const Record& rec : dataset) {
		writer.write(QStringList() << rec.date.toString("yyyy-MM-dd") << rec.name << rec.map << rec.winner << rec.winnerRace << rec.loser << rec.loserRace);
	}
	qDebug() << "Export completed.";
	return res;
}
