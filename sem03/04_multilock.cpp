#include <iostream>
#include <thread>
#include <mutex>

struct Account {
    std::mutex mtx;
    int balance;
    explicit Account(int initial) : balance(initial) {}
};

Account A{100000};
Account B{100000};

void transfer(Account& from, Account& to, int amount) {
    if (&from == &to) return;

    std::lock(from.mtx, to.mtx);

    from.balance -= amount;
    to.balance   += amount;

    from.mtx.unlock();
    to.mtx.unlock(); // в этом виде разблокировку нужно снять самостоятельно
}

void worker_ab() {
    for (int i = 0; i < 1000000; ++i) transfer(A, B, 1);
}

void worker_ba() {
    for (int i = 0; i < 1000000; ++i) transfer(B, A, 1);
}

int main() {
    std::thread t1(worker_ab);
    std::thread t2(worker_ba);

    t1.join();
    t2.join();

    std::cout << "A.balance=" << A.balance
              << "  B.balance=" << B.balance
              << "  total=" << (A.balance + B.balance)
              << '\n';
}