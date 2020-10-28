#include<mpi.h>
#include <vector>
#include <sstream>
#include "utils.h"


/* Программа вычисляет произведение матриц C = A*B параллельно,
 * используя заранее подготовленное разбиение матриц A и B из входных файлов*/
int main(int argc, char **argv) {
    /* Переменные для мастер процесса
     * dims[2] - количество процессов в каждом измерении топологии 2D решётка
     * periods[2] - периодичность решётки (все значения = 0 - отсутствует)
     * rowCountC - количество строк в итоговой матрице С
     * columnCountC - количество столбцов в итоговой матрице С*/
    int dims[2], periods[2], rowCountC, columnCountC;
    /* size - сумарное число процессов
     * rank - номер процесса в стартовом коммуникаторе
     * comm - коммуникатор для топологии 2D решётка
     * status - необходим для проверки статуса операции MPI_Recv*/
    int size, rank;
    MPI_Comm comm;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    /* инициализация periods для создания 2D решётки*/
    for (int i = 0; i < 2; i++) {
        periods[i] = 0;
    }
    /* Вычисляем число процессов в каждом измерении решётки.
     * Можно использовать MPI_Dims_create для поиска более близских делителей общего числа процессов,
     * но так как данные разбивает программа DataPreprocessor, то метод разбиения должен совпадать с методом из DataPreprocessor*/
    createDims(size, dims);
    /* Инициализируем топологию с коммуникатором comm */
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm);
    /* Мастер процесс читает данные из входных файлов и распределяет данные по процессам */
    if (rank == 0) {
        rowCountC = 0;
        columnCountC = 0;
        /* mSub - вектор хранящий прочитанную из файла матрицу (подматрицу)*/
        std::vector<double> mSub = std::vector<double>();
        /* Распределяем подматрицы A по процессам так,
         * что бы различным координатам в измерении dims[0] соответствовали различные подматрицы A*/
        for (int i = 0; i < dims[0]; i++) {
            mSub.clear();
            int rowCount, columnCount;
            /*Читаем матрицу из файла A_i.txt*/
            std::stringstream filename = std::stringstream();
            filename << "A_" << i << ".txt";
            readMatrix(filename.str().c_str(), rowCount, columnCount, mSub);
            /*Аккумулируем число строк всех извлеченных подматриц A. По итогу получаем число строк матрицы C.*/
            rowCountC += rowCount;
            /* Распределяем подматрицу A по всем процессам с координатой i в измерении dims[0]*/
            for (int j = 0; j < dims[1]; j++) {
                /* Узнаём номер процесса в общем коммуникаторе по его координатам в 2D решётке*/
                int coord[2] = {i, j};
                int rankDim;
                MPI_Cart_rank(comm, coord, &rankDim);
                /* Отправляем процессу число строк/столбцов подматрицы A и саму подматрицу*/
                MPI_Send(&rowCount, 1, MPI_INT, rankDim, 0, MPI_COMM_WORLD);
                MPI_Send(&columnCount, 1, MPI_INT, rankDim, 1, MPI_COMM_WORLD);
                MPI_Send(mSub.data(), rowCount * columnCount, MPI_DOUBLE, rankDim, 2, MPI_COMM_WORLD);
            }
        }
        /* Распределяем подматрицы B по процессам так,
         * что бы различным координатам в измерении dims[1] соответствовали различные подматрицы B*/
        for (int i = 0; i < dims[1]; i++) {
            mSub.clear();
            int rowCount, columnCount;
            /*Читаем матрицу из файла B_i.txt*/
            std::stringstream filename = std::stringstream();
            filename << "B_" << i << ".txt";
            readMatrix(filename.str().c_str(), rowCount, columnCount, mSub);
            columnCountC += rowCount;
            for (int j = 0; j < dims[0]; j++) {
                /* Узнаём номер процесса в общем коммуникаторе по его координатам в 2D решётке*/
                int coord[2] = {j, i};
                int rankDim;
                MPI_Cart_rank(comm, coord, &rankDim);
                /* Отправляем процессу число строк/столбцов подматрицы B и саму подматрицу*/
                MPI_Send(&rowCount, 1, MPI_INT, rankDim, 3, MPI_COMM_WORLD);
                MPI_Send(&columnCount, 1, MPI_INT, rankDim, 4, MPI_COMM_WORLD);
                MPI_Send(mSub.data(), rowCount * columnCount, MPI_DOUBLE, rankDim, 5, MPI_COMM_WORLD);
            }
        }
    }

    /* Код получения подматрицы A, B их умножения и возврата результата умножения
     * в мастер процесс в виде подматрицы C
     * Код выполняется во всех процессах*/
    /* Получаем подматрицу A адресованную данному процессу*/
    int rowCountA, columnCountA;
    MPI_Recv(&rowCountA, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&columnCountA, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    double *subMA = new double[rowCountA * columnCountA];
    MPI_Recv(subMA, rowCountA * columnCountA, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, &status);
    /* Получаем подматрицу B адресованную данному процессу*/
    int rowCountB, columnCountB;
    MPI_Recv(&rowCountB, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, &status);
    MPI_Recv(&columnCountB, 1, MPI_INT, 0, 4, MPI_COMM_WORLD, &status);
    double *subMB = new double[rowCountB * columnCountB];
    MPI_Recv(subMB, rowCountB * columnCountB, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD, &status);
    /* subMC - результирующую подматрицу C
     * Размер зарезервированной памяти для subMC в мастер процессе должен быть равен максимальному размеру subMC
     * из всех процессов, так как потом эта же память будет использоваться для получения матриц subMC из остальных процессов*/
    double *subMC;
    if (rank == 0) {
        subMC = new double[(rowCountC - (rowCountC / dims[0]) * (dims[0] - 1)) *
                           (columnCountC - (columnCountC / dims[1]) * (dims[1] - 1))];
    } else {
        subMC = new double[rowCountA * rowCountB];
    }
    /* Перемнажаем подматрецы A и B  и кладём результат в подматрецу C
     * Так как subMB это транспонированная (в программе DataPreprocessor) подматрица B,
     * то в ней мы итерируемся по строкам, а не по столбцам*/
    for (int i = 0; i < rowCountA; i++) {
        for (int j = 0; j < rowCountB; j++) {
            subMC[i * rowCountB + j] = 0;
            for (int k = 0; k < columnCountA; k++) {
                subMC[i * rowCountB + j] += subMA[columnCountA * i + k] * subMB[columnCountA * j + k];
            }
        }
    }
    /* Отправляем матрицу subMC в мастер процесс*/
    MPI_Send(subMC, rowCountA * rowCountB, MPI_DOUBLE, 0, 6, MPI_COMM_WORLD);
    /* Мастер процесс получает от других процессов подсчитанные ими подматрицы итоговой матрицы C
     * и записывает результат в файл C.txt*/
    if (rank == 0) {
        /* bn1 - число строк подматриц A*/
        /* bn3 - число строк подматриц B*/
        /* В случае, если число строк/столбцов не делиться нацело на число процессов в соответствующем измерении решётки,
         * то последний блок:
         * строк A будет равен bn1 + остаток от деления rowCountC на dims[0]
         * столбцов B будет равен bn3 + остаток от деления columnCountC на dims[1] */
        int bn1 = rowCountC / dims[0];
        int bn3 = columnCountC / dims[1];
        /* mC - результирующая матрица C*/
        double *mC = new double[columnCountC * rowCountC];
        for (int i = 0; i < dims[0]; i++) {
            for (int j = 0; j < dims[1]; j++) {
                /* Читаем подматрицу C от процесса с координатой (i,j)*/
                int coord[2] = {i, j};
                int rankDim;
                int subRowCountC = (i == dims[0] - 1 && rowCountC % dims[0] != 0) ? rowCountC - bn1 * (dims[0] - 1)
                                                                                  : bn1;
                int subColumnCountC = (j == dims[1] - 1 && columnCountC % dims[1] != 0) ? columnCountC -
                                                                                          bn3 * (dims[1] - 1) : bn3;
                MPI_Cart_rank(comm, coord, &rankDim);
                MPI_Recv(subMC, subRowCountC * subColumnCountC, MPI_DOUBLE, rankDim, 6, MPI_COMM_WORLD, &status);
                /* Вставляем подматрицу C в итоговую матрицу С*/
                addSubMatrix(mC, subMC, bn3 * j, bn1 * i, subColumnCountC, subRowCountC, columnCountC);
            }
        }
        /* Пишем результат умножения в файл C.txt*/
        writeMatrix("C.txt", mC, columnCountC, 0, rowCountC);
        delete[] mC;
    }

    MPI_Comm_free(&comm);
    MPI_Finalize();
    delete[] subMA;
    delete[] subMB;
    delete[] subMC;
    return (0);
}