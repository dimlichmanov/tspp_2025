#include <iostream>
#include <thread>
#include <mutex>

struct Resource {
    Resource() { std::cout << "Resource constructed\n"; }
    void use()   { std::cout << "use() on " << std::this_thread::get_id() << std::endl; }
};

std::once_flag g_once;
Resource*      g_res = nullptr; 

void init_resource() {
    g_res = new Resource; 
}

void worker() {
    std::call_once(g_once, init_resource); 
    g_res->use();                          
}

int main() {
    std::thread t1(worker);
    std::thread t2(worker);

    t1.join();
    t2.join();

    delete g_res;
    return 0;
}

// Ниже - вариант через static переменную, которая всегда инициализируется атомарно !

// #include <iostream>
// #include <thread>

// struct Resource {
//     Resource() { std::cout << "Resource constructed\n"; }
//     void use() { std::cout << "use() on " << std::this_thread::get_id() << std::endl; }
// };

// Resource& get_resource() {
//     static Resource r;
//     return r;
// }

// void worker() {
//     get_resource().use();
// }

// int main() {
//     std::thread t1(worker);
//     std::thread t2(worker);
//     t1.join();
//     t2.join();
// }

