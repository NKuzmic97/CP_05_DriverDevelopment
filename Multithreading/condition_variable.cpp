#include <iostream>
#include <thread>
#include <fstream>
#include <deque>
#include <mutex>
#include <condition_variable>

std::deque<int> q;
std::mutex mut;
std::condition_variable cond;

void f1(){
	int count = 10;
	while (count > 0) {
		std::unique_lock<std::mutex> locker(mut);
		q.push_front(count);
		locker.unlock();

		cond.notify_one(); // Notify the sleeping thread

		std::this_thread::sleep_for(std::chrono::seconds(1));
		count--;
	}
}

void f2(){
	int data = 0;

	while(data != 1){
		std::unique_lock<std::mutex> locker(mut);

		cond.wait(locker,[](){ return !q.empty();}); // spurious wake

		data = q.back();
		q.pop_back();
		locker.unlock();
		std::cout << "T2 got a value from T1: " << data << std::endl;
	}
}

int main(){
	std::thread t1(f1);
	std::thread t2(f2);

	t1.join();
	t2.join();

	return 0;
}
