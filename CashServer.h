#pragma once
#include "Cash.h"

#include <condition_variable>
#include <mutex>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <iostream> 
#include <fstream>
#include <zlib.h>

class CashServer
{
public:
	explicit CashServer(const std::string& RootPath,int MemorySize,int TimeToUpdate);
	~CashServer()=default;

	void UpdateCash();
	void DisplayFileContent(std::filesystem::path PathToShow);
private:
	std::mutex Mutex_;

	void CheckPath(std::filesystem::path& PathToShow) const;
	std::ifstream* CheckCash(std::filesystem::path& PathToShow);

	std::unordered_map<std::string,Cash> Cash_{};

	int Pack(Cash* Cash);
	int Unpack(Cash* Cash);

	std::filesystem::path Root_;


	int TimeToUpdate_;
	int MemorySize_{};
};