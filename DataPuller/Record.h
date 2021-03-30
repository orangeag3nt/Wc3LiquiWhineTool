#pragma once

#include <QString>
#include <QDate>

struct Record
{
	QString name;
	QDate date;

	QString map;
	QString winner;
	QString winnerRace;
	QString loser;
	QString loserRace;
};