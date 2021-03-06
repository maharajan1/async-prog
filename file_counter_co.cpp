#include <iostream>
#include <future>
#include <boost/filesystem.hpp>
#include <boost/chrono.hpp>

using namespace std;
using namespace boost::filesystem;
using namespace boost::chrono;

future<size_t> count_files_pure_async(const path &src)
{
	try
	{
		if (is_directory(src)) {
			size_t count = 0;
			for (directory_entry& item : directory_iterator(src)) {
				count += co_await count_files_pure_async(item.path());
			}

			co_return count;
		}
		else if (is_regular_file(src))
		{
			co_return ((size_t)1);
		}
	}
	catch (const std::exception& ex)
	{
	}

	co_return ((size_t)0);
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		std::cout << "please pass directory to recursively count on files" << std::endl;
		return -1;
	}

	auto t1 = high_resolution_clock::now();
	auto count = count_files_pure_async(argv[1]).get();
	auto t2 = high_resolution_clock::now();

	std::cout << count << ", time = " << duration_cast<seconds>(t2 - t1).count() << std::endl;

	return 0;
}
