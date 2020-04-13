#include <iostream>
#include <thread>
#include <future>
#include <deque>

// Packaged task can link callable function object to a future.

/* Summary - 3 ways to get a future:
std::promise::get_future();
std::packaged_task::get_future();
std::async() returns a future.
*/

int factorial(int N) {
	int res = 1;
	for (int i = N; i > 1; i--)
		res *= i;

	std::cout << "Result is: " << res << std::endl;
	return res;
}

std::deque<std::packaged_task<int()>> task_q;
std::mutex mu;
std::condition_variable cond;

void thread_2() {
	std::packaged_task<int()> t;
	{
		std::unique_lock<std::mutex> locker(mu);
		cond.wait(locker, []() { return !task_q.empty(); });
		t = std::move(task_q.front());
		task_q.pop_front();
	}
		t();
}

int main() {
	// std::thread t1(factorial, 6);
	std::thread t2(thread_2);
	std::packaged_task<int()> t(std::bind(factorial,6));

	std::future<int> fu = t.get_future();

	{
		std::lock_guard<std::mutex> locker(mu);
		task_q.push_back(std::move(t));
	}

	cond.notify_one();

	// auto b = std::bind(factorial, 6);

	// Not possible when using std::bidn in packaged_task creation
	// t(6); // In a different context than context of creation

	// int x = t.get_future().get(); // Get return value from packaged task

	// b();

	std::cout << fu.get() << std::endl;
	t2.join();

	return 0;
}