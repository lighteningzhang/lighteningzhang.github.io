#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

using namespace std;

constexpr int MAX_SIZE = 1;

mutex mtx;
condition_variable full;
condition_variable empty;
int cur_num = 0;

int data[MAX_SIZE];

void producer(int id) {

    unique_lock<mutex> lock(mtx);
    while (cur_num == MAX_SIZE) empty.wait(lock);

    // 获得锁，并用while判断休眠的条件，标准操作！


    cout << "produces " << id << " produce!" << endl;
    cur_num ++;
    full.notify_one();
    lock.unlock(); // 先释放锁，再发信号，反过来可能在这两个之间调度了，但是那个线程又立刻睡眠了。
    

}

void consumer(int id) {
    unique_lock<mutex> lock(mtx);
    while (cur_num == 0) full.wait(lock);

    // 获得锁，并用while判断休眠的条件，标准操作！

    cout << "consumer " << id << " consume!" << endl;
    cur_num --;
    empty.notify_one();
    lock.unlock();  // 先释放锁，再发信号，反过来可能在这两个之间调度了，但是那个线程又立刻睡眠了。
    
}

int main() {

    vector<thread> prods, cons;
    for (int i = 0; i < 10; i ++) {
        prods.emplace_back(producer, i);
    }
    for (int i = 0; i < 10; i ++) {
        cons.emplace_back(consumer, i);
    }
    for (auto &x : prods) x.join();
    for (auto &x : cons) x.join();
    cout << "all done!" << endl;
    return 0;
}