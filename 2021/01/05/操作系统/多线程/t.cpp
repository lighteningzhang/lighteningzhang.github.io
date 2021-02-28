#include <thread>
#include <vector>
#include <mutex>
#include <iostream>
#include <condition_variable>
using namespace std;

std::mutex mtx;
std::condition_variable cv;

char arr[] = { 'a','b','c' };
char message = 'a';

void test(int i) {
    for(int j = 0 ;j < 10;++j){
        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [=] {return message == arr[i]; });
        std::cout << arr[i];
        message = arr[(i + 1) % 3];
        // lk.unlock();
        cv.notify_all();
    }
}

int main()
{
    std::thread t[3];
    for (int i = 0; i < 3; ++i) {
        t[i] = std::thread(test,i);
    }

    for (int i = 0; i < 3; ++i) {
        t[i].join();
    }
    std::cout << std::endl;
    return 0;
}