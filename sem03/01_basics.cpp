#include <thread>
#include <iostream>

int main() {
    std::thread t([]{ std::cout << "Hello from thread\n"; });
    t.join();
}

// Ниже - код с data race

// #include <thread>
// #include <iostream>
// #include <functional>

// void add10(int& x) {    
//         x+= 10;
// }

// int main() {
//     int a = 5;

//     std::thread t1([&]{ a+=10; /*std::cout  << a << std::endl;*/ });
//     std::thread t2(add10, std::ref(a));
    
//     t1.join(); t2.join();
//     std::cout << "a after t2 = " << a << "\n"; 
// }
