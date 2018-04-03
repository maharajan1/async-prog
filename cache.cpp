// g++-6 -std=c++14 -I ../boost_1_65_1/stage/include cache.cpp -o cache -L../boost_1_65_1/stage/lib -lboost_thread -lboost_chrono -lboost_system -lpthread

#define BOOST_THREAD_PROVIDES_FUTURE
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION 

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <boost/thread/thread.hpp>
#include <boost/thread/future.hpp>

using namespace boost;

std::vector<std::shared_ptr<thread>> gThreads;

class Cache
{
public:
	future<int> get(std::string key)
	{
		if (key == "cached")
		{
			std::cout << "Found in cache!" << std::endl;
			return make_ready_future(100);
		}

		std::cout << "Not Found in cache!" << std::endl;
		auto promisePtr = std::make_shared<promise<int>>();
		auto threadPtr = std::make_shared<thread>(Cache::fetcher, key, promisePtr);
		gThreads.push_back(threadPtr);
		return promisePtr->get_future();
	}

private:
	static void fetcher(std::string key,
		std::shared_ptr<promise<int>> promise)
	{
		this_thread::sleep_for(chrono::seconds(2));
		std::cout << "fetching key " << key << " ..." << std::endl;
		if (key == "error")
		{
			promise->set_exception(std::runtime_error("Oops! Error"));
		}
		else
		{
			promise->set_value(10);
		}
	}
};

int main()
{
	Cache cache;
	auto f = cache.get("key");
	std::cout << f.get() << std::endl;

	Cache cache1;
	auto f1 = cache1.get("key");
	while (!f1.is_ready())
	{
		std::cout << "Result not available, doing something ..." << std::endl;
		f1.wait_for(chrono::seconds(1));
	}
	std::cout << f1.get() << std::endl;

	Cache cache3;
	auto f2 = cache3.get("key");
	f2.then([](future<int> value) {
		try
		{
			std::cout << "Value is " << value.get() << std::endl;
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
		}
	});

	for (auto thread : gThreads)
		thread->join();

	char ch;
	std::cin >> ch;

	return 0;
}
