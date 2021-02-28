#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <condition_variable>
using namespace std;
int items = 0;
const int MX = 2;
mutex mtx;
condition_variable empty, full;
void produce(int id) {
    unique_lock<mutex> lock(mtx);
    while (items == MX) full.wait(lock);
    //用while而不是if的原因是避免群惊，就算误唤醒了某些线程，
    //它再回过头来检测自己是否能拿到锁，没拿到则继续等
    //
    cout << "producer id: " << id << endl;
    items ++;
    
    // lock.unlock();
    empty.notify_all();
}

void consume(int id) {
    unique_lock<mutex> lock(mtx);
    while (items == 0) empty.wait(lock);
    cout << "consumer id: " << id << endl;
    items --;
    
    // lock.unlock();
    full.notify_all();
}
int main () {
    vector<thread> prod, con;
    for (int i = 0; i < 10; i ++) prod.emplace_back(produce, i);
    
    for (int i = 0; i < 10; i ++) con.emplace_back(consume, i);
    
    for (auto &p : prod) p.join();
    for (auto &c : con) c.join();
    
    

    return 0;
}