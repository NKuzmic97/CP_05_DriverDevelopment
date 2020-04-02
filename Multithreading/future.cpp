#include <future>
#include <thread>
#include <iostream>

int factorial(std::shared_future<int> f){
	int res = 1;
	int n = f.get(); // if we forget - exception thrown: std::future_errc::broken_promise

	for(int i=n;i>1;i--)
		res*=i;

	std::cout << "Result is: " << res << std::endl;
	return res;
}

int main() {
	int x;
	//std::thread t1(factorial,4,std::ref(x));
	//t1.join();

	std::promise<int> p;
	std::future<int> f = p.get_future();
	std::shared_future<int> sf = f.share();

	std::future<int> fu = std::async(std::launch::async,factorial,sf); //if std::launch::deffered - Not async, one thread.
	std::future<int> fu2 = std::async(std::launch::async,factorial,sf);
	//std::future<int> fu3 = std::async(std::launch::async,factorial,sf);
	// std::future<int> fu4 = std::async(std::launch::async,factorial,sf);
	// ... 10 threads for example. We cannot share to same future with all the threads.
	// Do something else...

	std::this_thread::sleep_for(std::chrono::milliseconds(20));

	//p.set_exception(std::make_exception_ptr(std::runtime_error("To error is human.")));
	p.set_value(4);

	x = fu.get(); // You can call get from future ONLY once.

	std::cout << "Result from child is: " << x << std::endl;

	return 0;
}
