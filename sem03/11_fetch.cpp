#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include <cstdint>

int main() {
    const unsigned T = std::thread::hardware_concurrency() ? std::thread::hardware_concurrency() : 4;
    const uint64_t N = 1000000; 

    std::atomic<uint64_t> counter{0};
    std::vector<std::thread> th;

    for (unsigned t = 0; t < T; ++t) {
        th.emplace_back([&]{
            for (uint64_t i = 0; i < N; ++i) {
                counter.fetch_add(1); 
            }
        });
    }
    for (auto &i : th) i.join();

    std::cout << counter.load() << std::endl;
}