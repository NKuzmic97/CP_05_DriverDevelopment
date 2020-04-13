#include <iostream>
#include <mutex>
#include <thread>
#include <future>
#include <chrono>

int factorial(int N) {
	int res = 1;
	for (int i = N; i > 1; i--)
		res *= i;

	std::cout << "Result is: " << res << std::endl;
	return res;
}

int main() {
	/* Thread */
	std::thread t1(factorial, 6);
	std::this_thread::sleep_for(std::chrono::milliseconds(3));
	auto tp = std::chrono::steady_clock::now() + std::chrono::microseconds(4);
	std::this_thread::sleep_until(tp);

	/* Mutex */
	std::mutex mu;
	std::lock_guard<std::mutex> locker(mu);
	std::unique_lock<std::mutex> uniq_locker(mu);
	uniq_locker.try_lock();
	uniq_locker.try_lock_for(std::chrono::nanoseconds(500));
	uniq_locker.try_lock_until(tp);

	/* Condition variable */
	std::condition_variable cond;
	cond.wait_for(uniq_locker, std::chrono::microseconds(2));
	cond.wait_until(uniq_locker, tp);

	/* Future and promise */
	std::promise<int> p;
	std::future<int> f = p.get_future();
	f.get();
	f.wait();
	f.wait_for(std::chrono::milliseconds(1));
	f.wait_until(tp);

	/* async() */
	std::future<int> fut = std::async(factorial, 6);
	int x = fut.get();

	/* Packaged task */
	std::packaged_task<int(int)> t(factorial);
	std::future<int> fut2 = t.get_future();
	t(6);
}
