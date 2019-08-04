#pragma once
#include <fstream>
struct Cash
{
	Cash(std::ifstream* File, const unsigned int Time):File(File),Time(Time){};
	Cash()=default;
	std::ifstream* File=nullptr;
	unsigned int Time=0;
	bool IsPacked=false;
};