#pragma once

#include "Record.h"
#include <QList>
#include <QObject>
#include <QDate>

class QNetworkReply;
struct RecordLoaderPrivate;

class RecordLoader : public QObject
{
	Q_OBJECT
public:
	RecordLoader();
	~RecordLoader();
	void start(const QString& patch, const QDate& minDate, const QDate& maxDate);
	QList<Record> result() const;

private:
	void slotNetworkReplyFinished(QNetworkReply* reply);
	void nextIteration();
	void finish();

	RecordLoaderPrivate* d;

signals:
	void signalFinished();
};
