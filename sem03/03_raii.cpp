#include <mutex>
#include <thread>
#include <vector>
#include <iostream>

std::mutex m;
int counter = 0;
const int N_threads = 4;

void worker(int n) {
    for (int i = 0; i < n; ++i) {
        std::lock_guard<std::mutex> lock(m);
        ++counter;
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


// Поток с thread_num = 1 не отпустит мьютекс при выбросе исключения в примере ниже -> всегда делаем lock_guard

// #include <mutex>
// #include <thread>
// #include <vector>
// #include <iostream>

// std::mutex m;
// int counter = 0;
// const int N_threads = 4;

// void bad_worker(int n, int thread_num) {
//     for (int i = 0; i < n; ++i) {
//         //std::lock_guard<std::mutex> lg(m);

//         m.lock();
//         if (thread_num == 1 && i == 12345) {
//             throw std::runtime_error("Error in thread!");
//         }
//         ++counter;
//         m.unlock();
//     }

// } 

// int main() {
//     const int per_thread = 10000000;

//     std::vector<std::thread> pool;
//     for (int i = 0; i < N_threads; ++i) {
//         pool.emplace_back([i](){ 
//             try{bad_worker(per_thread, i);} 
//             catch(const std::exception& e) { std::cout << "caught " << e.what() << std::endl;}
//             });
//     }

//     for (auto& t: pool) t.join();

//     std::cout << "counter = " << counter << std::endl;
// }

