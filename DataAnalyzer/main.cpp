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
	//top50
	static QList<QString> players = QList<QString>()
		<< "Happy"
		<< "Moon"
		<< "Lyn"
		<< "Hitman"
		<< "Colorful"
		<< "Eer0"
		<< "LawLiet"
		<< "XlorD"
		<< "FoCuS"
		<< "Chaemiko"
		<< "Foggy"
		<< "Sok"
		<< "Infi"
		<< "Fly100%"
		<< "Fortitude"
		<< "CrunCher"
		<< "TeD"
		<< "VortiX"
		<< "Soin"
		<< "TH000"
		<< "Starbuck"
		<< "Lin Guagua"
		<< "Pink"
		<< "15sui"
		<< "ToD"
		<< "WaN"
		<< "HawK"
		<< "Kaho"
		<< "KraV"
		<< "Blade"
		<< "Ice orc"
		<< "MichaeL"
		<< "XiaoKai"
		<< "Neytpoh"
		<< "Fast"
		<< "KiWiKaKi"
		<< "ThundeR"
		<< "Pcg 123"
		<< "LucifroN"
		<< "JohnnyCage"
		<< "Tbc bm"
		<< "Sini"
		<< "Alice"
		<< "TeRRoR"
		<< "INSUPERABLE"
		<< "Yange"
		<< "HuG"
		<< "WFZ"
		<< "Zhou Xixi"
		<< "Cash"
		;
	return players.contains(name);
}

bool isValidMap(const QString& map) {
	static QList<QString> maps = QList<QString>()
		<< "Tidehunters"
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
		const int neededColumns = 5;
		int shift = items.count() - neededColumns;
		if (shift < 0) {
			qDebug() << "Record count < 5.";
			return 1;
		}

		Record r;
		r.map = items[shift];
		r.winner = items[shift + 1];
		r.winnerRace = items[shift + 2];
		r.loser = items[shift + 3];
		r.loserRace = items[shift + 4];
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