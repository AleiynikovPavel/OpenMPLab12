#include <iostream>
#include <vector>
#include <sstream>
#include <cstring>
#include "utils.h"


/* Программа чтает матрицу A из файла A.txt и B из файла B.txt
 * На вход в программу подаётся число процессов, которые будут вычислять произведение A * B
 * Программа распределяет процессы в 2D решётке и определяет подматрицы A и B для каждой координаты в решётке
 * Итоговые подматрицы записываются в файлы: A_0.txt, A_1.txt, ..., A_i.txt и B_0.txt, B_1.txt, ..., B_j.txt */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Invalid number of arguments\n" << std::endl;
        return -1;
    }
    int pCount = std::stoi(argv[1]);
    if (pCount < 2) {
        std::cerr << "Invalid number of processor\n" << std::endl;
        return -1;
    }
    /* mA - вектор с матрицей A прочитанной из файла*/
    std::vector<double> mA = std::vector<double>();
    /* mB - вектор с матрицей B прочитанной из файла*/
    std::vector<double> mB = std::vector<double>();
    /* n1 - число строк в A
     * n2 - число колонок в A
     * n3 - число колонок в B
     * n4 - число строк в B*/
    int n1, n2, n3, n4;
    readMatrix(strdup("A.txt"), n1, n2, mA);
    readMatrix(strdup("B.txt"), n4, n3, mB);
    if (n2 != n4 || n1 == 0 || n2 == 0 || n3 == 0) {
        std::cerr << "Matrix incorrect size\n" << std::endl;
        return -1;
    }
    /* mBT - транспонированная матрица B. Так удобнее разбивать её по процессам.*/
    std::vector<double> mBT = std::vector<double>(mB.size(), 0);
    transpose(mB, mBT, n2, n3);
    /* Определяем число процессов в каждом измерении 2D решетки*/
    int dims[2];
    createDims(pCount, dims);
    std::cout << "Dims: " << dims[0] << " " << dims[1] << std::endl;
    /* bn1 - число строк подматриц A*/
    /* bn3 - число строк подматриц B*/
    /* В случае, если число строк/столбцов не делиться нацело на число процессов в соответствующем измерении решётки,
     * то последний блок:
     * строк A будет равен bn1 + остаток от деления n1 на dims[0]
     * столбцов B будет равен bn3 + остаток от деления n3 на dims[1] */
    int bn1 = n1 / dims[0];
    for (int i = 0; i < dims[0]; i++) {
        /* Разбиваем мактрицу A на блоки построчно и записываем в файл A_i.txt*/
        int rows = (i == dims[0] - 1 && n1 % dims[0] != 0) ? n1 - bn1 * (dims[0] - 1) : bn1;
        std::stringstream filename = std::stringstream();
        filename << "A_" << i << ".txt";
        writeMatrix(filename.str().c_str(), mA.data(), n2, i * bn1, rows);
    }
    int bn3 = n3 / dims[1];
    for (int i = 0; i < dims[1]; i++) {
        /* Разбиваем мактрицу B на блоки построчно и записываем в файл B_i.txt*/
        int rows = (i == dims[1] - 1 && n3 % dims[1] != 0) ? n3 - bn3 * (dims[1] - 1) : bn3;
        std::stringstream filename = std::stringstream();
        filename << "B_" << i << ".txt";
        writeMatrix(filename.str().c_str(), mBT.data(), n2, i * bn3, rows);
    }
    return 0;
}

