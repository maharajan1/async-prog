//  g++-6 -std=c++14 -I ../boost_1_65_1/stage/include file_counter.cpp -DPURE_ASYNC -o file_counter -L../boost_1_65_1/stage/lib -lboost_thread -lboost_chrono -lboost_system -lboost_filesystem -lpthread

#define BOOST_THREAD_VERSION 4
#define BOOST_THREAD_PROVIDES_EXECUTORS
#define BOOST_THREAD_PROVIDES_FUTURE 
#define BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION 
#define BOOST_THREAD_PROVIDES_FUTURE_WHEN_ALL_WHEN_ANY 

#include <iostream>
#include <vector>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread/futures/wait_for_all.hpp>
#include <boost/thread/executors/basic_thread_pool.hpp>
#include <boost/thread/executors/executor_adaptor.hpp>
#include <boost/thread/executor.hpp>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::chrono;

future<size_t> count_files_pure_async_n(const path &src)
{
	static boost::executor_adaptor<boost::basic_thread_pool> ea;
	try
	{
		if (is_directory(src)) {
			std::list<future<size_t>> results;
			for (directory_entry& item : directory_iterator(src)) {
				results.push_back(count_files_pure_async_n(item.path()));
			}

			auto resFuture = when_all(results.begin(), results.end());
			return resFuture.then(ea, [](auto results) {
				auto rs = results.get();
				size_t count = 0;
				for (auto &f : rs)
				{
					count += f.get();
				}

				return count;
			});
		}
		else if (is_regular_file(src))
		{
			boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
			return make_ready_future((size_t)1);
		}
	}
	catch (const std::exception& ex)
	{
	}

	return make_ready_future((size_t)0);
}

future<size_t> count_files_pure_async(const path &src)
{
	try
	{
		if (is_directory(src)) {
			std::list<future<size_t>> results;
			for (directory_entry& item : directory_iterator(src)) {
				results.push_back(count_files_pure_async(item.path()));
			}

			auto resFuture = when_all(results.begin(), results.end());
			return resFuture.then(
				boost::launch::deferred,
				[](auto results) {
				auto rs = results.get();
				size_t count = 0;
				for (auto &f : rs)
				{
					count += f.get();
				}

				return count;
			});
		}
		else if (is_regular_file(src))
		{
			return make_ready_future((size_t)1);
		}
	}
	catch (const std::exception& ex)
	{
	}

	return make_ready_future((size_t)0);
}

size_t count_files_async_ex(const path &src)
{
	static boost::executor_adaptor<boost::basic_thread_pool> ea;
	try
	{
		if (is_directory(src)) {
			std::list<future<size_t>> results;
			for (directory_entry& item : directory_iterator(src)) {
				results.push_back(async(ea,
					count_files_async_ex,
					item.path()));
			}

			wait_for_all(results.begin(), results.end());
			size_t count = 0;
			for (auto &f : results)
			{
				count += f.get();
			}

			return count;
		}
		else if (is_regular_file(src))
		{
			return (size_t)1;
		}
	}
	catch (const std::exception& ex)
	{
	}

	return (size_t)0;
}

size_t count_files_async(const path &src)
{
	try
	{
		if (is_directory(src)) {
			std::list<future<size_t>> results;
			for (directory_entry& item : directory_iterator(src)) {
				results.push_back(async(boost::launch::async,
					count_files_async,
					item.path()));
			}

			wait_for_all(results.begin(), results.end());
			size_t count = 0;
			for (auto &f : results)
			{
				count += f.get();
			}

			return count;
		}
		else if (is_regular_file(src))
		{
			return (size_t)1;
		}
	}
	catch (const std::exception& ex)
	{
	}

	return (size_t)0;
}

size_t count_files_sync(const path &src)
{
	try
	{
		if (is_directory(src)) {
			size_t count = 0;
			for (directory_entry& item : directory_iterator(src)) {
				count += count_files_sync(item.path());
			}

			return count;
		}
		else if (is_regular_file(src)) {
			boost::this_thread::sleep_for(boost::chrono::microseconds(1));
			return 1;
		}
	}
	catch (const std::exception& ex)
	{
	}

	return 0;
}

int main(int argc, const char *argv[]) {

	if (argc < 2)
	{
		std::cout << "please pass directory to recursively count on files" << std::endl;
		return -1;
	}

	size_t result = 0;
	auto t1 = high_resolution_clock::now();

#ifdef SYNC
	result = count_files_sync(argv[1]);
#endif

#ifdef ASYNC
	result = count_files_async(argv[1]);
#endif

#ifdef AYSNC_EX
	result = count_files_async_ex(argv[1]);
#endif

#ifdef PURE_ASYNC
	result = count_files_pure_async(argv[1]).get();
#endif

	auto t2 = high_resolution_clock::now();
	std::cout << "file count = " << result << "\n"
		<< "time taken (sec) = " << duration_cast<seconds>(t2 - t1).count()
		<< std::endl;

	return 0;
}
