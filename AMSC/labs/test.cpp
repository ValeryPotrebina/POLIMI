#include <iostream>
#include <array>
#include <vector>

int main() {
    int a = 10;
    double b = 10.0;

    auto c = a + b;
    std::cout << typeid(c).name() << std::endl;

    double d = static_cast<double>(a);
    std::cout << d << std::endl;

    
    return 0;
}