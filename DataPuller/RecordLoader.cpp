#include "RecordLoader.h"
#include "CsvReader.h"
#include <QHash>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QTextStream>

const int Limit = 500;
const int MaxOffset = 5000;

struct Direction
{
	Direction() = delete;
	enum Type {
		Asc, Desc
	};
	static QString toString(Type t) { return t == Asc ? "asc" : "desc"; }
};

struct RecordLoaderPrivate
{
	QString patch;
	QHash<QString, Record> records;
	QNetworkAccessManager networkManager;
	bool isWorking = false;
	int nextOffset;
	Direction::Type direction;
};

RecordLoader::RecordLoader() : QObject()
{
	d = new RecordLoaderPrivate();
	connect(&d->networkManager, &QNetworkAccessManager::finished, this, &RecordLoader::slotNetworkReplyFinished);
}

RecordLoader::~RecordLoader()
{
	delete d;
}

QList<Record> RecordLoader::result() const
{
	return d->records.values();
}

void RecordLoader::start(const QString& patch)
{
	if (d->isWorking)
		return;
	d->isWorking = true;
	d->patch = patch;
	d->direction = Direction::Asc;
	d->nextOffset = 0;
	QTimer::singleShot(0, this, &RecordLoader::nextIteration);
}

void RecordLoader::nextIteration()
{
	QNetworkRequest request;
	QString url = "https://liquipedia.net/warcraft/Special:Ask/mainlabel%3D/format%3Dcsv/sort%3DHas-20map-20date/order%3D" + Direction::toString(d->direction) +
		"/offset%3D" + QString::number(d->nextOffset) + "/limit%3D" + QString::number(Limit) +"/" \
		"-5B-5B-2DHas-20subobject::-20-3Cq-3E-5B-5BCategory:Individual-20Tournaments-5D-5D-3C-2Fq-3E-20-5D-5D-20-5B-5B:%2B-5D-5D-20-5B-5BHas-20patch::Patch-20" + d->patch +
		"-5D-5D-20-5B-5BHas-20map-20date::%2B-5D-5D/-3FIs-20played-20on/-3FHas-20winner-20page/-3FHas-20winning-20race/-3FHas-20loser-20page/" \
		"-3FHas-20losing-20race/prettyprint%3Dtrue/unescape%3Dtrue/searchlabel%3DCSV";
	request.setUrl(QUrl(url));
	d->networkManager.get(request);
}

void RecordLoader::slotNetworkReplyFinished(QNetworkReply* r)
{
	r->deleteLater();
	if (r->error() != QNetworkReply::NoError) {
		qDebug() << r->errorString();
		emit signalFinished();
		return;
	}

	QTextStream stream(r);
	stream.setCodec("UTF-8");
	CsvReader reader(&stream);
	reader.read(); //skip headers
	bool sameRecords = false;
	while (!reader.atEnd()) {
		QStringList row = reader.read();
		Record r;
		r.name = row[0];
		r.map = row[1];
		r.winner = row[2];
		r.winnerRace = row[3];
		r.loser = row[4];
		r.loserRace = row[5];
		r.offset = d->nextOffset;//~~
		if (d->records.contains(r.name)) {
			Record old = d->records.value(r.name);
			qDebug() << old.offset << r.offset;
			sameRecords = true;
		}
		d->records.insert(r.name, r);
	}
	qDebug() << d->nextOffset << Direction::toString(d->direction) << "same" << sameRecords;

	if (sameRecords) {
		d->isWorking = false;
		emit signalFinished();
		return;
	}

	d->nextOffset += Limit;
	if (d->nextOffset >= MaxOffset) {
		if (d->direction == Direction::Asc) {
			d->direction = Direction::Desc;
			d->nextOffset = 0;
		} else {
			qDebug() << "Too much records in liqui. Can't access to all.";
			d->isWorking = false;
			emit signalFinished();
			return;
		}
	}
	QTimer::singleShot(0, this, &RecordLoader::nextIteration);
}

