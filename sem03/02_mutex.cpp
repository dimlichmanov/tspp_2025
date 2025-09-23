#include <mutex>
#include <thread>
#include <vector>
#include <iostream>

std::mutex m;
int counter = 0;
const int N_threads = 4;

void worker(int n) {
    for (int i = 0; i < n; ++i) {
        m.lock();
        ++counter;
        m.unlock();
    }
}

int main() {
    const int per_thread = 10000000;

    std::vector<std::thread> pool;
    for (int i = 0; i < N_threads; ++i) {
        pool.emplace_back(worker, per_thread);
    }

    for (auto& t : pool) t.join();

    std::cout << "counter = " << counter << std::endl;
}