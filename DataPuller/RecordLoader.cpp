#include "RecordLoader.h"
#include "CsvReader.h"
#include <QHash>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QTextStream>

const int Limit = 500;

struct RecordLoaderPrivate
{
	QString patch;

	QHash<QString, Record> records;
	QNetworkAccessManager networkManager;
	bool isWorking = false;
	QDate nextDate;
	QDate maxDate;
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

void RecordLoader::start(const QString& patch, const QDate& minDate, const QDate& maxDate)
{
	if (d->isWorking)
		return;
	d->isWorking = true;
	d->patch = patch;
	d->nextDate = minDate;
	d->maxDate = maxDate;
	QTimer::singleShot(0, this, &RecordLoader::nextIteration);
}

void RecordLoader::nextIteration()
{
	QNetworkRequest request;
	request.setRawHeader("User-Agent", "I_am_extracting_data_to_CSV (github.com/orangeag3nt/Wc3LiquiWhineTool)");
	request.setRawHeader("Accept-Encoding", "gzip");
	QLocale l("en_US");
	QString day = QString::number(d->nextDate.day());
	QString month = l.monthName(d->nextDate.month());
	QString year = QString::number(d->nextDate.year());
	QString url = "https://liquipedia.net/warcraft/Special:Ask/mainlabel%3D/format%3Dcsv/limit%3D" + QString::number(Limit) +
		"/-5B-5B-2DHas-20subobject::-20-3Cq-3E-5B-5BCategory:Individual-20Tournaments-5D-5D-3C-2Fq-3E-20-5D-5D-20-5B-5B:%2B-5D-5D-20-5B-5BHas-20patch::Patch-20" +
		d->patch + "-5D-5D-20-5B-5BHas-20map-20date::%E2%89%A5" + day + "-20" + month + "-20" + year +
		"-2000:00:00-5D-5D-20-5B-5BHas-20map-20date::%E2%89%A4" + day + "-20" + month + "-20" + year +
		"-2023:59:59-5D-5D/-3FIs-20played-20on/-3FHas-20winner-20page/-3FHas-20winning-20race/-3FHas-20loser-20page/-3FHas-20losing-20race";
	request.setUrl(QUrl(url));
	d->networkManager.get(request);
}

void RecordLoader::finish()
{
	d->isWorking = false;
	emit signalFinished();
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
	int count = 0;
	while (!reader.atEnd()) {
		QStringList row = reader.read();
		Record r;
		r.date = d->nextDate;
		r.name = row[0];
		r.map = row[1];
		r.winner = row[2];
		r.winnerRace = row[3];
		r.loser = row[4];
		r.loserRace = row[5];
		if (d->records.contains(r.name)) {
			Record old = d->records.value(r.name);
			qDebug() << "FAIL!!! dublicate record" << r.name << "at date:" << d->nextDate;
			finish();
			return;
		}
		d->records.insert(r.name, r);
		count++;
	}

	if (count >= Limit) {
		qDebug() << "FAIL!!! DANGEROUS DATE (too many records) at date" << d->nextDate;
		finish();
		return;
	}

	qDebug() << d->nextDate << "successfully added" << count << "records";
	d->nextDate = d->nextDate.addDays(1);
	if (d->nextDate > d->maxDate) {
		finish();
		return;
	}

	QTimer::singleShot(30000, this, &RecordLoader::nextIteration);
}

