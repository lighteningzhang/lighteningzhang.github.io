#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
using namespace std;
mutex mtx;
condition_variable fob;
string ch[2] = {"foo", "bar"};
int cur = 0;
void printFoo(int k) {
    unique_lock<mutex> lk(mtx);
    for (int i = 0; i < 10; i ++) {
        // lk.lock();
        while (cur % 2 != k) {
            fob.wait(lk);
        }
        cout << ch[k];
        cur = (cur + 1) % 2;
        // lk.unlock();
        fob.notify_all();
    }

}

int main () {
    vector<thread> vt;
    for (int i = 0; i < 2; i ++)
        vt.emplace_back(printFoo, i);
    
    for (int i = 0; i < 2; i ++)
        vt[i].join();
    cout << endl;
    return 0;
}