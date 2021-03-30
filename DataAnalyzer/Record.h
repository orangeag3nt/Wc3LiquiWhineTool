#pragma once

#include <QString>

struct Race
{
	enum Type
	{
		Unknown,
		Human,
		Orc,
		NightElf,
		Undead
	};

	static Type fromString(const QString& s);
};

struct Record
{
	QString map;
	QString winner;
	/*Race::Type*/QString winnerRace;
	QString loser;
	/*Race::Type*/QString loserRace;
};