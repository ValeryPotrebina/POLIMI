#include <iostream>
#include <limits>

int main() {
    // Машинный эпсилон для разных типов
    constexpr float eps_f = std::numeric_limits<float>::epsilon();
    constexpr double eps_d = std::numeric_limits<double>::epsilon();
    
    std::cout << "Float epsilon: " << eps_f << std::endl;
    std::cout << "Double epsilon: " << eps_d << std::endl;
    
    // Проверка точности
    float x = 1.0f;
    float y = x + eps_f;
    std::cout << "1.0 + eps = " << y << std::endl;
    std::cout << "y > 1.0: " << (y > 1.0f) << std::endl;
    
    return 0;
}