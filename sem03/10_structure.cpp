#include <atomic>
#include <thread>
#include <iostream>
#include <type_traits>
#include <vector>

struct Pair {
    int a;
    int b;
};

static_assert(std::is_trivially_copyable_v<Pair>);

std::atomic<Pair> atom{ Pair{0, 0} };

// ЭТО неправильно! Пока делаем инкременты, другие потоки могут обновить атомик

void inc_by2(std::atomic<Pair>& my_atom_pair) {
    Pair var = my_atom_pair.load();
    var.a += 2;
    var.b += 2;
    my_atom_pair.store(var);
}

// Это уже правильно
void inc_by2_CAS(std::atomic<Pair>& my_atom_pair) {
    Pair var = my_atom_pair.load();
    Pair new_var;
    do 
    {
        new_var.a = var.a + 2;
        new_var.b = var.b + 2;
    } while (!my_atom_pair.compare_exchange_strong(var, new_var));
}


int main() {

    auto inc_lambda = []{
       for (int i = 0; i < 1000000; i++) {
            inc_by2_CAS(atom);
       }
    };

    // Проверьте, поддерживается ли ваш атомик аппаратно, либо же нет (тогда C++ вам сам обернёт все операции под мьютексы) 
    std::cout << atom.is_lock_free() << std::endl;

    std::vector<std::thread> th;

    for (int i = 0; i < 4; i++) {
        th.emplace_back(inc_lambda);
    }

    for (int i = 0; i < 4; i++) {
        th[i].join();
    }

    auto my_final_pair = atom.load();

    std::cout << my_final_pair.a << " " << my_final_pair.b << std::endl;
}









