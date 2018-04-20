#include <iostream>
#include <future>
#include <boost/chrono.hpp>
#include <string>
#include <thread>

using namespace std;
using namespace boost::chrono;

string getHello()
{
	std::cout << "getting Hello " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(10s);
	return "Hello";
}

string getWorld()
{
	std::cout << "getting World " << std::this_thread::get_id() << std::endl;
	std::this_thread::sleep_for(15s);
	return "World";
}

future<string> getHelloWorld()
{
	auto hello = co_await std::async(std::launch::async, getHello);
	auto world = co_await std::async(std::launch::async, getWorld);

	return hello + " " + world;
}

future<string> getHelloWorldEx()
{
	auto hello = std::async(std::launch::async, getHello);
	auto world = std::async(std::launch::async, getWorld);
	auto h = co_await hello;
	auto w = co_await world;

	return h + " " + w;
}

int main(int argc, char* argv[])
{
	auto t1 = high_resolution_clock::now();
	auto result = getHelloWorld().get();
	auto t2 = high_resolution_clock::now();

	std::cout << result << ", time = " << duration_cast<seconds>(t2 - t1).count() << std::endl;

	t1 = high_resolution_clock::now();
	result = getHelloWorldEx().get();
	t2 = high_resolution_clock::now();

	std::cout << result << ", time = " << duration_cast<seconds>(t2 - t1).count() << std::endl;
	
	return 0;
}
