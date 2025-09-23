#include <iostream>

int main() {

    int sum = 0; 

    int some_var = 9;
    auto add_to_sum = [&sum, some_var](int x) { sum += x ; sum += some_var; };
    some_var = 100;
    add_to_sum(5);
    
    std::cout << sum << std::endl;
   
}