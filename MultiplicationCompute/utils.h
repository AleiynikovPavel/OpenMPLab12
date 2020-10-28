#ifndef LAB12MC_UTILS_H
#define LAB12MC_UTILS_H


#include <vector>


/* Метод для разбиения n процессов в 2D решетке (аналог MPI_Dims_create)
 * IN n - число процессов
 * OUT dims - двухмерный массив.
 *   dims[0] - число процессов в первом измерении
 *   dims[1] - число процессов во втором измерении
 * Например для n = 4, dims[0] = 2, dims[1] = 2*/
void createDims(int n, int * dims);
/* Метод читает матрицу из файла
 * IN filename - имя файла
 * OUT row - число строк в прочитанной матрице
 * OUT column - число столбцов в прочитанной матрице
 * OUT M - вектор содержащий прочитанную матрицу*/
void readMatrix(const char * filename, int & row, int & column, std::vector<double> & M);
/* Метод пишет матрицу в файла
 * IN M - матрица
 * IN column - число столбцов в матрице
 * IN offset - смещение по строчкам (если offset = 1, то первая строчка не будет записана в файл)
 * IN row - число строк, которое следует записать в файл начиная с offset*/
void writeMatrix(const char * filename, double * M, int column, int offset, int row);
/* Метод вставляет подматрицу в матрицу
 * OUT M - матрица в которую будут вставляться данные
 * IN subM - подматрица которую надо вставить
 * IN pos_x - координата (колонка) в которой будет находится левого верхнего угла подматрицы
 * IN pos_y - координата (строка) в которой будет находится левого верхнего угла подматрицы
 * IN row_size - число столбцов в матрице subM
 * IN row_count - число строк в матрице subM
 * m_row_size row - число столбцов в матрице M*/
void addSubMatrix(double * M, const double * subM, int pos_x, int pos_y, int row_size, int row_count, int m_row_size);

#endif
