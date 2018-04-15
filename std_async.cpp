//g++ -std=c++11 -pthread std_async.cpp -o main -lpthread

#include <iostream>
#include <future>
#include <vector>
#include <numeric>

using namespace std;

int accumulate_ret(int* data, size_t count) {
  return std::accumulate(data, data + count, 0);
}
void use_worker_in_std_async() {
  std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
  future<int> fut = async(
      std::launch::async, accumulate_ret, v.data(), v.size());
  cout << "Result " << fut.get() << endl;
}

using int_futures = std::vector<std::future<int>>;
int_futures launch_async(std::vector<int>& v) {
  int_futures futures;
  futures.push_back(async(std::launch::async, accumulate_ret,
                          v.data(), v.size() / 2));
  futures.push_back(async(std::launch::async, accumulate_ret,
                          v.data() + v.size() / 2, v.size() / 2));
  return futures;
}

int main()
{
    use_worker_in_std_async();
    
    std::vector<int> v{1, 2, 3, 4, 5, 6, 7, 8};
    int_futures futs = launch_async(v);
    cout << "Result : " << futs[0].get() + futs[1].get() << endl;

    return 0;
}
