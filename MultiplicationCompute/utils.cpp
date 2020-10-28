#include "utils.h"
#include <fstream>


void createDims(int n, int * dims) {
    dims[0] = 1;
    dims[1] = 1;
    bool set_first = true;
    int d = 2;
    while (d * d <= n) {
        if (n % d == 0) {
            if (set_first) {
                dims[0] *= d;
            } else {
                dims[1] *= d;
            }
            set_first = !set_first;
            n /= d;
        } else {
            d += 1;
        }
    }
    if (n > 1) {
        if (set_first) {
            dims[0] *= n;
        } else {
            dims[1] *= n;
        }
    }
}

void readMatrix(const char * filename, int & row, int & column, std::vector<double> & M) {
    column = 0;
    row = 0;
    std::ifstream file;
    file.open(filename);
    if (!file.is_open()) {
        return;
    }
    double a;
    while(!file.eof()) {
        file >> a;
        M.push_back(a);
        if (row == 0) {
            column++;
        }
        if (file.peek() == '\n' || file.eof()) {
            row++;
        }
    }
}

void writeMatrix(const char * filename, double * M, int column, int offset, int row) {
    std::ofstream file;
    file.open(filename);
    if (!file.is_open()) {
        return;
    }
    for (int i = offset; i < row+offset; i++) {
        for (int j = 0; j < column; j++) {
            if (j != 0) {
                file << ' ';
            }
            file << M[i*column+j];
        }
        if (i + 1 < row+offset) {
            file << '\n';
        }
    }
    file.close();
}

void addSubMatrix(double * M, const double * subM, int pos_x, int pos_y, int row_size, int row_count, int m_row_size) {
    for (int i = pos_y; i < row_count+pos_y; i++) {
        for (int j = pos_x; j < row_size+pos_x; j++) {
            M[i*m_row_size+j] = subM[(i-pos_y)*row_size+j-pos_x];
        }
    }
}
