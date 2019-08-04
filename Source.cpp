#include "CashServer.h"

#include <iterator>
#include <string>
#include <iostream>
#include <future>

int main()
{
	CashServer Server("P:\\CashServer\\res",256,5);

	auto Async=std::async(std::launch::async,&CashServer::UpdateCash,&Server);

	using In = std::istream_iterator<std::string>;
	std::for_each(In(std::cin), In(), [&Server=Server](const std::string& FileName)
	{
		Server.DisplayFileContent(FileName);
	});

	return 0;
}
