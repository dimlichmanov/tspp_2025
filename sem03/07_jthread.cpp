// ex2.cpp
#include <thread>
#include <stop_token>
#include <chrono>
#include <iostream>


// Собирать с С++ 20, если есть

void worker(std::stop_token st) {
    using namespace std::chrono_literals;
    while (!st.stop_requested()) {
        std::cout << "tick\n";
        std::this_thread::sleep_for(200ms);
    }
    std::cout << "stop requested, exiting" << std::endl;
}

int main() {
    using namespace std::chrono_literals;
    std::jthread t(worker);
    std::this_thread::sleep_for(1s);
    t.request_stop();
}