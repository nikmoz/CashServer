#include "CashServer.h"

CashServer::CashServer(const std::string& RootPath, const int MemorySize, const int TimeToUpdate):TimeToUpdate_(TimeToUpdate),MemorySize_(MemorySize)
{
	Root_=std::filesystem::path(RootPath);
}


void CashServer::UpdateCash()
{
	while(true)
	{

		for (auto& Cash : Cash_)
		{
			const auto Current = clock() / 1000.0;
			if (Current - Cash.second.Time > static_cast<unsigned int>(TimeToUpdate_))
			{
				//std::cout << "Time's up, lets do this" << '\n';
				Cash.second.Time = static_cast<unsigned int>(Current);
				
				//MemorySize_ -= Pack(&Cash.second);


				Cash.second.IsPacked = true;
			}
		}
	}
}

void CashServer::DisplayFileContent(std::filesystem::path PathToShow)
{
	std::lock_guard<std::mutex>Lock (Mutex_);
	try
	{
		CheckPath(PathToShow);

		std::cout << "Displaying content for: " << PathToShow << "\n";

		const auto File=CheckCash(PathToShow);

		if(File!=nullptr)
		{
			File->clear();
			File->seekg(0, std::ifstream::beg);

			std::string Line;

			while (std::getline(*File,Line))
			{
				std::cout<<Line;
			}
			std::cout<<std::endl;
		}
	}
	catch(const char* Msg)
	{
		std::cerr << Msg << std::endl;
		if (std::string(Msg)=="Not enough memory")
		{
			std::ifstream TmpFile(PathToShow);
			std::string Line;
		
			while (std::getline(TmpFile,Line))
			{
				std::cout<<Line;
			}
			std::cout<<std::endl;
		}
	}
}

void CashServer::CheckPath(std::filesystem::path& PathToShow) const
{
	if(PathToShow.parent_path()!=Root_)
	{
		if(PathToShow.parent_path()=="")
		{
			PathToShow=Root_/PathToShow;
		}

		else {return throw("Root directory isn't server"); }
	}
}

std::ifstream* CashServer::CheckCash(std::filesystem::path& PathToShow)
{
	if (Cash_.find(PathToShow.string())!=Cash_.end())
	{
		//std::cout<<"Get It"<<std::endl;
		if (Cash_[PathToShow.string()].IsPacked)
		{
			//MemorySize_+=Unpack(&Cash_[PathToShow.string()]);
			//
			//auto New=Cash_[PathToShow.string()];

			Cash_[PathToShow.string()].IsPacked=false;
		}
		return Cash_[PathToShow.string()].File;
	}
	if (std::filesystem::exists(PathToShow)&&std::filesystem::is_regular_file(PathToShow))
	{
		const auto Size=static_cast<unsigned int>(std::filesystem::file_size(PathToShow))+sizeof(Cash);

		if (MemorySize_-static_cast<int>(Size)>0)
		{
			//std::cout<<"Oh, a new one"<<std::endl;

			MemorySize_-=Size;
			Cash_[PathToShow.string()]= Cash(new std::ifstream(PathToShow.string()),0);

			return Cash_[PathToShow.string()].File;
		}
		throw("Not enough memory");
	}
	throw("Can't resolve path to file");
}

int CashServer::Pack(Cash* Cash)
{
	std::lock_guard<std::mutex> Lock(Mutex_);

	Cash->File->clear();
	Cash->File->seekg(0, std::ifstream::beg);

	const auto OldSize=sizeof(*(Cash->File));

	auto CompressSize=compressBound(OldSize);

	auto Buffer=reinterpret_cast<Bytef*>(Cash->File);

	compress(Buffer,&CompressSize,Buffer,OldSize);

	Cash->File=reinterpret_cast<std::ifstream*>(Buffer);

	const auto NewSize=sizeof(*(Cash->File));

	return NewSize-OldSize;
}

int CashServer::Unpack(Cash* Cash)
{
	std::lock_guard<std::mutex> Lock(Mutex_);

	const auto OldSize=sizeof(*(Cash->File));

	uLongf DecompressSize=sizeof(*(Cash->File));

	auto Buffer=reinterpret_cast<Bytef*>(Cash->File);

	uncompress(Buffer,&DecompressSize,Buffer,OldSize);

	const auto NewSize=sizeof(*(Cash->File));

	Cash->File=reinterpret_cast<std::ifstream*>(Buffer);

	return NewSize-OldSize;
}