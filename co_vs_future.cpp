/* output 
6460: invoking Dns lookup ...
6720:Looking DNS server for www.google.com ...
6460: Watching TV ...
6460: Watching TV ...
6460: Watching TV ...
6460: Watching TV ...
6460: Watching TV ...
2316: got ip address after DNS lookup 123.49.21.1
6720:Fetching data from server 123.49.21.1 ...
6460: Watching TV ...
6460: Watching TV ...
6460: Watching TV ...
6460: Watching TV ...
6460: Watching TV ...
4368: got data from server 123.49.21.1 : Result
6460: Got data from server 123.49.21.1 : Result
6460: Good Bye...
*/

#include <future>
#include <iostream>
#include <thread>
#include <string>

std::string lookupDNS(std::string dns)
{
	std::cout << std::this_thread::get_id() << ":" << "Looking DNS server for " << dns << " ..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return "123.49.21.1";
}

std::string fetchDataFromIp(std::string ip)
{
	std::cout << std::this_thread::get_id() << ":" << "Fetching data from server " << ip << " ..." << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return ip + " : Result";
}

std::future<std::string> getDataFromDns(std::string dns)
{
	std::cout << std::this_thread::get_id() << ":" << " invoking Dns lookup ... " << std::endl;
	std::string ip = co_await std::async(std::launch::async, lookupDNS, dns);
	std::cout << std::this_thread::get_id() << ":" << " got ip address after DNS lookup " << ip << std::endl;
	std::string data = co_await std::async(std::launch::async, fetchDataFromIp, ip);
	std::cout << std::this_thread::get_id() << ":" << " got data from server " << data << std::endl;
	co_return data;
}

int main()
{
	auto result = getDataFromDns("www.google.com");
	while (!result._Is_ready()) {
		std::cout << std::this_thread::get_id() << ":" << " Watching TV ... " << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	std::cout << std::this_thread::get_id() << ":" << " Got data from server " << result.get() << std::endl;
	std::cout << std::this_thread::get_id() << ":" << " Good Bye... " << std::endl;
    return 0;
}

