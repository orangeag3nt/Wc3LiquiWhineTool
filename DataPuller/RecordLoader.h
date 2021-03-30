#pragma once

#include "Record.h"
#include <QList>
#include <QObject>

class QNetworkReply;
struct RecordLoaderPrivate;

class RecordLoader : public QObject
{
	Q_OBJECT
public:
	RecordLoader();
	~RecordLoader();
	void start(const QString& patch);
	QList<Record> result() const;

private:
	void slotNetworkReplyFinished(QNetworkReply* reply);
	void nextIteration();

	RecordLoaderPrivate* d;

signals:
	void signalFinished();
};
