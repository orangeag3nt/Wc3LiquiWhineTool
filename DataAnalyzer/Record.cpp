#include "Record.h"


Race::Type Race::fromString(const QString& s)
{
	if (s == "n")
		return NightElf;
	else if (s == "h")
		return Human;
	else if (s == "o")
		return Orc;
	else if (s == "u")
		return Undead;
	return Unknown;
}