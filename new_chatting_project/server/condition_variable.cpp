#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>

std::mutex mtx;
std::condition_variable cv;
bool ready = false;

void print_id(int id) {
    std::unique_lock<std::mutex> lck(mtx);
    while (!ready) {
        cv.wait(lck);
    }
    std::cout << "Thread " << id << " is working" << std::endl;
}

void go() {
    std::unique_lock<std::mutex> lck(mtx);
    ready = true;
    cv.notify_all();
}

int main() {
    std::thread threads[10];
    // spawn 10 threads:
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(print_id, i);
    }
    std::cout << "10 threads ready to race..." << std::endl;
    go(); // go!

    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
