#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <unistd.h>
#include <assert.h>
#include <cstdio>
#include <ctime>

using namespace std;

mutex mtx;
condition_variable cv_writer; // 写者等待的信号量
condition_variable cv_reader; // 读者等待的信号量

int cnt_reader; // 表示正在读的读者的个数
int cnt_wait_write;  // 表示正在等待写的写者的个数
bool cnt_writting;  // 表示是否正在有写者在写

time_t base;  // 整个程序开始的时间
time_t cur_time;  // 存储当前时间


/**
 * 读者的过程
 * @param id 线程ID
 * @param begin_time 开始执行时间
 * @param run_time 持续时间
 */
void reader(int id, double begin_time, double run_time) {

    sleep(begin_time);
    printf("reader %d want read! | time %ld \n", id, time(&cur_time) - base);
    flush(cout);
    unique_lock<mutex> lock(mtx);
    while (cnt_wait_write != 0 || cnt_writting) cv_reader.wait(lock);
    assert(cnt_wait_write == 0 && cnt_writting == 0);
    cnt_reader ++;
    lock.unlock();

    printf("reader %d begin read! | time %ld  \n", id, time(&cur_time) - base);
    flush(cout);

    sleep(run_time);

    lock.lock();
    cnt_reader --;
    printf("reader %d exit! | | time %ld \n", id, time(&cur_time) - base);
    if (cnt_wait_write != 0 && cnt_reader == 0) {
        cv_writer.notify_one();
    } else {
        cv_reader.notify_all();
    }
    lock.unlock();

}

/**
 * 写者的过程
 * @param id 线程id
 * @param begin_time 开始执行时间
 * @param run_time 持续时间
 */
void writer(int id, double begin_time, double run_time) {

    sleep(begin_time);
    printf("writer %d want write! | time %ld  \n", id, time(&cur_time) - base);
    flush(cout);

    unique_lock<mutex> lock(mtx);
    cnt_wait_write ++; // 标记有写者来了。
    while (cnt_reader != 0 || cnt_writting) cv_writer.wait(lock);
    assert(cnt_reader == 0 && !cnt_writting);
    cnt_writting = true;
    cnt_wait_write --;
    lock.unlock();

    printf("writer %d begin write! | time %ld \n", id, time(&cur_time) - base);
    flush(cout);
    sleep(run_time);

    lock.lock();
    cnt_writting = false;
    printf("writer %d exit! | | time %ld \n", id, time(&cur_time) - base);

    if (cnt_wait_write != 0) {
        cv_writer.notify_one();  // 只有一个写者可以写，所以只唤醒一个写者
    } else{
        cv_reader.notify_all();  // 因为所有的读者可以同时读，所以将所有的读者都唤醒
    }
    lock.unlock();
}

// while (hasWriter || wrter > 0)
int main() {

    freopen("./input.txt","r",stdin);  // 标准输入重定向到文件

    time(&base);  // 获取整个进程开始的时间

    vector<thread> all_threads;
    int n;
    cin >> n;
    while (n --) {
        int id;
        char type;
        double begin_time;
        double run_time;
        cin >> id >> type >> begin_time >> run_time;
        if (type == 'R') {
            all_threads.emplace_back(reader, id, begin_time, run_time);
        } else {
            all_threads.emplace_back(writer, id, begin_time, run_time);
        }
    }
    for (auto &x : all_threads) x.join();

    cout << "all done!" << endl;
    return 0;
}