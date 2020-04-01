#include <iostream>
#include <fstream>
#include <thread>
#include <string>
#include <mutex>

/* This is a problem because cout is global - some other thread can try to access it. 

std::mutex mut;

void shared_print(string msg, int id){
	std::lock_guard<std::mutex> guard(mut); // RAII
	//mut.lock();
	std::cout << msg << id << std::endl; // what if this throws exceptions? - mutex will never be unlocked
	//mut.unlock();
}

// Initial problem:
// we get mixed outputs - little from t1 - little from main thread
// this is because our threads are racing for a resource - stdcout
// we resolve that by using a common function and locking that function using mutex

*/

// Instead, we can bind mutex with the resource we are protecting.

class LogFile {
private:
	std::mutex m_mutex;
	std::ofstream f;
public:
	LogFile(){
		f.open("log.txt");
	} // Need a destructor to close the file.

	void shared_print(std::string id, int value){
		std::lock_guard<std::mutex> locker(m_mutex);
		f << "From " << id << ": " << value << std::endl;
	}

	~LogFile(){
		f.close();
	}

	// Never return f to the outside world.
	// eg. ofstream& GetStream() { return f;}
	// Never pass f as an argument to user provided function
	// void processf(void fun(ofstream&)){
	// fun(f);
	// }
};

/* class Stack{
	int* _data = nullptr;
	std::mutex _mut;
public:
	void pop(); // pops off the item on top of the stack
	int& top(); // returns item on top by ref
}; */


// is this code thread safe? -nope.
/* void function_2(class Stack& st){
	int v = st.top();
	st.pop();
	process(v);
}*/

void function_1(LogFile& log) {
	for(int i=0;i>-100;i--)
		log.shared_print(std::string("T1: "),i);
}

int main(){
	LogFile log;
	std::thread t1(function_1,std::ref(log));

	for(int i=0;i<100;i++)
		log.shared_print(std::string("Main thread: "),i);

	t1.join();

	return 0;
}

