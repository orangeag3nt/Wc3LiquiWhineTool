#include "CsvReader.h"
#include <QTextStream>

enum CsvState {
	UnquotedField,
	QuotedField,
	QuotedQuote
};

struct CsvReaderPrivate
{
	QTextStream* stream;
	CsvState state;
	QString currentValue;
};

CsvReader::CsvReader(QTextStream* base)
{
	d = new CsvReaderPrivate();
	d->stream = base;
	d->state = UnquotedField;
}

CsvReader::~CsvReader()
{
	delete d;
}

QStringList CsvReader::read()
{
	QStringList items;

	d->currentValue = QString();
	if (atEnd())
		return items;
	do {
		QChar c = d->stream->read(1).at(0);

		switch (d->state)
		{
		case UnquotedField:
			if (c == ',') {
				items << d->currentValue;
				d->currentValue = QString();
			} else if (c == '"') {
				d->state = CsvState::QuotedField;
			} else if (c == '\n') {
				d->state = CsvState::UnquotedField;
				items << d->currentValue;
				d->currentValue = QString();
				return items;
			} else if (c != '\r') {
				d->currentValue += c;
			}
			break;
		case QuotedField:
			if (c == '"') {
				d->state = QuotedQuote;
			} else if (c != '\r') {
				d->currentValue += c;
			}
			break;
		case QuotedQuote:
			if (c == ',') {
				items << d->currentValue;
				d->currentValue = QString();
				d->state = UnquotedField;
			} else if (c == '\n') {
				items << d->currentValue;
				d->currentValue = QString();
				d->state = UnquotedField;
				return items;
			} else if (c != '\r') {
				d->currentValue += c;
				d->state = c == '"' ? CsvState::QuotedField : CsvState::UnquotedField;
			}
			break;
		}
	} while (!atEnd());

	items << d->currentValue;
	return items;
}

bool CsvReader::atEnd()
{
	return d->stream->atEnd();
}
