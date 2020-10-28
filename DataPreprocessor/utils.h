#ifndef LAB12_UTILS_H
#define LAB12_UTILS_H

#include <vector>


/* Метод для разбиения n процессов в 2D решетке (аналог MPI_Dims_create)
 * IN n - число процессов
 * OUT dims - двухмерный массив.
 *   dims[0] - число процессов в первом измерении
 *   dims[1] - число процессов во втором измерении
 * Например для n = 4, dims[0] = 2, dims[1] = 2*/
void createDims(int n, int * dims);
/* Метод пишет матрицу в файла
 * IN M - матрица
 * IN column - число столбцов в матрице
 * IN offset - смещение по строчкам (если offset = 1, то первая строчка не будет записана в файл)
 * IN row - число строк, которое следует записать в файл начиная с offset*/
void writeMatrix(const char * filename, double * M, int column, int offset, int row);
/* Метод читает матрицу из файла
 * IN filename - имя файла
 * OUT row - число строк в прочитанной матрице
 * OUT column - число столбцов в прочитанной матрице
 * OUT M - вектор содержащий прочитанную матрицу*/
void readMatrix(char * filename, int & row, int & column, std::vector<double> & M);
/* Метод для транспонирования матрицы
 * IN m - входная матрица
 * OUT mt - транспонированная матрица
 * OUT rows - число строк в m
 * OUT columns - число столбцов в m*/
void transpose(const std::vector<double> & m, std::vector<double> & mt, int rows, int columns);

#endif
