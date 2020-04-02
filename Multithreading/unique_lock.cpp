#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <mutex>

class LogFile {
private:
	std::mutex m_mutex;
	//std::mutex m_mutex_open;
	std::once_flag flag;
	std::ofstream f;
public:
	/* void shared_print(std::string id, int value){
		std::unique_lock<std::mutex> locker(m_mutex, std::defer_lock);
		// Do something that doesn't need mutex

		locker.lock();
		f << "From " << id << ": " << value << std::endl;
		locker.unlock();
		// We need to do something..

		std::unique_lock<mutex> locker2 = std::move(locker);
		// Lock guard cannot be moved.
	} */

	void shared_print(std::string id, int value){
		/* {
			std::unique_lock<std::mutex> locker2(m_mutex_open);
			if(!f.is_open()) {
				// Lazy initialization
				f.open("log.txt");
			}
		}*/

		std::call_once(flag,[&](){ f.open("log.txt");}); // file will be opened only once by one thread

		std::unique_lock<std::mutex> locker(m_mutex, std::defer_lock);
		f << "From " << id << ": " << value << std::endl;
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
