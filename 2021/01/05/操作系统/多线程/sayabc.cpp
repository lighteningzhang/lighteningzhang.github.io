#include <iostream>
#include <mutex>
#include <thread>
#include <condition_variable>
using namespace std;
int f = -1;
condition_variable a, b, c;
mutex mtx;
int cnt = 1;
void print1(char ch) {
    unique_lock<mutex> lock(mtx);
    while (cnt < 10) {
        while (f != 0) {
            cout << "print1" << endl;
            a.wait(lock);
        }
        printf("%c\n", ch);
        f ++;
        cnt ++;
        b.notify_one();
    }
}

void print2(char ch) {
    unique_lock<mutex> lock(mtx);
    while (cnt < 10) {
        while (f != 1) {
            cout << "print2" << endl;
            b.wait(lock);
        }
        printf("%c\n", ch);
        f ++; //加锁，且f = 1
        cnt ++;
        // lock.unlock();
        c.notify_one();
    }
}

void print3(char ch) {
    unique_lock<mutex> lock(mtx);
    while (cnt < 10) {
        while (f != 2) {
            
            cout << "print3" << endl;
            c.wait(lock);
        }
        printf("%c\n", ch);
        f = 0;
        cnt ++;   
        // lock.unlock();
        a.notify_one();
    }
}
int main () {
    thread t2(print2, 'b'), t1(print1, 'a'), t3(print3, 'c');
    f = 0;
    a.notify_all();
    t1.join();
    t2.join();
    t3.join();

    cout << endl;
    return 0;
}