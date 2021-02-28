#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

using namespace std;

int counter = 0;
mutex mtx;  // 全局大家都可见的锁

void increase(int cnt) {
	for (int i = 0; i < cnt; i ++) {
    mtx.lock();  // 尝试加锁
    counter ++;  // 每个线程不断地执行增1操作
    mtx.unlock();  // 释放锁
  } 
}

int main(int argc, char* argv[]) {
	vector<thread> threads;
	if (argc <= 1) {
		cout << "please input thread number" << endl;
		return 0;
	}	
	int num = atoi(argv[1]);
	int cnt = 100000;
	for (int i = 0; i < num; i ++) {
		threads.push_back(thread(increase, cnt));  // 创建若干线程，每个线程对全局变量进行增加操作
	}
	for (auto &x : threads) x.join();  // 等待所有线程执行完毕
	cout << "total num is " << counter << endl;  // 输出共享的全局变量最终的结果
	return 0;
}