#include "../DataPuller/CsvReader.h"
#include "Record.h"
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <QMap>

QMap<QString, int> wins;

bool isValidRace(const QString& race) {
	static QList<QString> races = QList<QString>() << "h" << "o" << "n" << "u";
	return races.contains(race);
}

bool isValidPlayer(const QString& name) {
	//top30 elo + TH000
	static QList<QString> players = QList<QString>()
		//multiple:"
		<< "15sui" << "infi"
		//humans:
		<< "th000" << "fortitude" << "chaemiko" << "sok" << "cruncher" << "hawk" << "johnnycage" << "blade"
		//uds:
		<< "happy" << "eer0" << "ted" << "vortix" << "michael" << "xlord" << "xiaokai" << "krav"
		//orcs:
		<< "lyn" << "fly100%" << "focus" << "hitman" << "soin" << "grubby" << "无道oc"
		//elves
		<< "moon" << "lawliet" << "foggy" << "colorful" << "sini" << "yange"
		;
	return players.contains(name.toLower());
}

bool isValidMap(const QString& map) {
	static QList<QString> maps = QList<QString>()
		<< "Terenas Stand LV"
		<< "Echo Isles"
		<< "Autumn Leaves"
		<< "Twisted Meadows"
		<< "Turtle Rock"
		<< "Last Refuge"
		<< "Amazonia"
		<< "Northern Isles"
		<< "Concealed Hill";
	return maps.contains(map);
}

int main(int argc, char *argv[])
{
	QFile f("e:/export.csv");
	if (!f.open(QIODevice::ReadOnly)) {
		qDebug() << f.errorString();
		return 666;
	}
	QTextStream stream(&f);
	stream.setCodec("UTF-8");
	CsvReader reader(&stream);
	reader.read(); //header
	while (!reader.atEnd()) {
		QStringList items = reader.read();
		if (items.count() != 5) {
			qDebug() << "Record count != 5.";
			return 1;
		}

		Record r;
		r.map = items[0];
		r.winner = items[1];
		r.winnerRace = items[2];
		r.loser = items[3];
		r.loserRace = items[4];
		if (!isValidRace(r.winnerRace) || !isValidRace(r.loserRace) ||
			!isValidPlayer(r.winner) || !isValidPlayer(r.loser) ||
			!isValidMap(r.map) || r.loserRace == r.winnerRace)
			continue;

		QString pair = r.winnerRace + r.loserRace;
		int count = wins[pair];
		wins[pair] = count + 1;
	}

	for (const QString& key : wins.keys()) {
		int winCount = wins[key];
		int loseCount = wins[QString(key[1]) + key[0]];
		int totalCount = winCount + loseCount;
		qDebug().noquote() << QString(key[0]).toUpper() + "x" + QString(key[1]).toUpper() << QString::number(100.0 * winCount / totalCount, 'f', 2) + "%"
			<< winCount << ":" << loseCount;
	}

	return 0;
}