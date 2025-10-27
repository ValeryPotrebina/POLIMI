#include "test.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char** argv) {
    cout << "========================================" << endl;
    cout << "  Sparse Matrix Reader with Eigen" << endl;
    cout << "========================================\n" << endl;
    
    // Определяем имя файла
    string filename = "test_matrix.mtx";
    
    // Если передан аргумент командной строки, используем его
    if (argc > 1) {
        filename = argv[1];
    }
    
    cout << "Reading file: " << filename << endl;
    
    // Создаем объект класса
    SparseMatrixReader reader(filename);
    
    // Загружаем матрицу
    if (!reader.loadMatrix()) {
        return 1;
    }
    
    // Выводим информацию о матрице
    reader.printInfo();
    
    // Выводим ненулевые элементы
    reader.printNonZeros();
    
    // Выводим матрицу в dense формате
    reader.printDense();
    
    return 0;
}