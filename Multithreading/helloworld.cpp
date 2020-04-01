#include <iostream>
#include <thread>

// shared resource of t1 and main thread is cout.
// main thread should run as long as cout is needed for thread t1

void function_1() {
	std::cout << "Beauty is only skin-deep." << std::endl;
}

class Functor {
public:
	void operator()(std::string msg) {
		std::cout << "T1 says: " << msg << std::endl;
		msg = "Trust is the mother of deceit."; // test line so we can check did thread change our message
		// std::cout << std::this_thread::get_id() << std::endl; // we can also use this to get children thread ID
}

};

int main(){
	std::string msg = "Where there is no trust, there is no love.";

	// each thread has it's own ID number
	std::thread::id mainThreadID = std::this_thread::get_id();
	std::cout << "Main thread ID: " << std::hex << mainThreadID << std::endl;

	std::thread t1((Functor()),std::move(msg)); // safe and efficient
	
	std::thread::id childThreadID = t1.get_id();
	std::cout << "Child thread ID: " << std::hex << childThreadID << std::endl;

	//std::thread t2 = t1; // doesn't work - copy constructor is deleted
	std::thread t2 = std::move(t1);

	t2.join(); // t1 is empty now

	// we can also pass the parameter by pointer
	// sharing the same memory causes data waste problem. what is data waste?
	// std::thread t1((Functor()), msg); // in this way, parameters to thread are always passed by value
	// std::thread t1((Functor()), std::ref(msg)); // this way we pass the msg by reference
	// this is causing threads to share the same memory
	// is synchronization needed?


	//std::cout << "From main: " << msg << std::endl;

	// Oversubscription is bad. Why?
	// When our program has more threads than our CPU, there is a lot of context switching.
	// A lot of context switching degrades perfomance.

	std::cout << "Available threads: " << std::thread::hardware_concurrency() << std::endl;

	return 0;
}