#pragma once

#include <QString>

/*struct Race
{
	enum Type
	{
		Unknown,
		Human,
		Orc,
		NightElf,
		Undead,
		Random
	};
};*/

struct Record
{
	QString name;

	QString map;
	QString winner;
	QString winnerRace; //todo: enum
	QString loser;
	QString loserRace; //todo: enum
};