#ifndef LABS_TEST_H
#define LABS_TEST_H

#include <Eigen/Sparse>
#include <string>
#include <iostream>

class SparseMatrixReader {
private:
    Eigen::SparseMatrix<double> matrix;
    std::string filename;

public:
    SparseMatrixReader(const std::string& file);

    // Метод для загрузки матрицы из файла
    bool loadMatrix();

    // Метод для вывода информации о матрице
    void printInfo() const;

    // Метод для вывода матрицы в dense формате
    void printDense() const;

    // Метод для вывода только ненулевых элементов
    void printNonZeros() const;

    // Геттер для получения матрицы
    const Eigen::SparseMatrix<double>& getMatrix() const;
};

#endif //LABS_TEST_H