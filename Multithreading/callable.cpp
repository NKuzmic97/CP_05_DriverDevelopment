#include <thread>

class A {
public:
	void f(int x,char c) { }
	long g(double x) { return 0; }
	int operator()(int n) { return 0; }
};

void foo(int x) { }

int main(){
	A a;

	std::thread t1(a,6); // copy of a in different thread
	std::thread t2(std::ref(a),6); // a in different thread
	std::thread t3(A(),6); // temporary A, will be moved into thread t3
	std::thread t4([](int x) { return x*x; }, 6); // providing a lambda for thread to execute
	std::thread t5(foo,7); // passing non member function
	std::thread t6(&A::f,a,8,'w'); // copy of a.f(8,'w') in different thread
	std::thread t7(&A::f,std::ref(a),9,'e'); // a.f(9'e') in different thread

	return 0;
}
