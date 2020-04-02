#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <mutex>

// Different order of mutex locks causes deadlock in the next situation.

class LogFile {
private:
	std::mutex m_mutex;
	std::mutex m_mutex2;
	std::ofstream f;
public:
	LogFile(){
		f.open("log.txt");
	} // Need a destructor to close the file.

	void shared_print(std::string id, int value){
		std::lock(m_mutex,m_mutex2);

		std::lock_guard<std::mutex> locker(m_mutex, std::adopt_lock);
		std::lock_guard<std::mutex> locker2(m_mutex2, std::adopt_lock);
		std::cout << "From " << id << ": " << value << std::endl;
	}

	void shared_print2(std::string id, int value){

		{
		std::lock_guard<std::mutex> locker(m_mutex);
		// Avoid locking a mutex and then calling a user function.
		// Prefer locking single mutex not multiple.
		// Use std::lock for multiple mutexes.
		// Lock the mutexes in the same order.
		// Introduce a hiearchy of mutexes.
		}

		{
		std::lock_guard<std::mutex> locker2(m_mutex2, std::adopt_lock);
		std::cout << "From " << id << ": " << value << std::endl;
		}
	}

	~LogFile(){
		f.close();
	}

};

void function_1(LogFile& log) {
	for(int i=0;i>-100;i--)
		log.shared_print(std::string("T1: "),i);
}

int main(){
	LogFile log;
	std::thread t1(function_1,std::ref(log));

	for(int i=0;i<100;i++)
		log.shared_print2(std::string("Main thread: "),i);

	t1.join();

	return 0;
}
