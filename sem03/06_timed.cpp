#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

std::timed_mutex m;

void worker() {
    std::unique_lock<std::timed_mutex> ul(m);
    std::this_thread::sleep_for(std::chrono::milliseconds(700));
}

int main() {
    std::thread t(worker);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));


    std::unique_lock<std::timed_mutex> ul(m, std::defer_lock);
    if (ul.try_lock_for(std::chrono::milliseconds(500))) {
        std::cout << "Got 500\n";
        m.unlock();
    }

    t.join();
}