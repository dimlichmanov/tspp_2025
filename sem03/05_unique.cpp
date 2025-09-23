#include <mutex>
#include <condition_variable>
#include <thread>
#include <iostream>

std::mutex m;
std::condition_variable cv;
bool ready = false;
int data = 0;

void producer() {
    {
        std::lock_guard<std::mutex> lg(m);
        data = 42;
        ready = true;
    }
    cv.notify_one();
}

void consumer() {
    std::unique_lock<std::mutex> ul(m); 
    cv.wait(ul, []{ return ready; }); 
    std::cout << "data = " << data << "\n";
}

int main() {
    std::thread t1(consumer), t2(producer);
    t1.join(); t2.join();
}
