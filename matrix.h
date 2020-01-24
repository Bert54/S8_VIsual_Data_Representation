#ifndef PROJET_MATRIX_H
#define PROJET_MATRIX_H

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <cmath>

using std::vector;
using std::tuple;

class Matrix {

private:
    unsigned m_rowSize;
    unsigned m_colSize;
    vector<vector<double> > m_matrix;
public:
    Matrix(unsigned, unsigned, double);
    Matrix(const char *);
    Matrix(const Matrix &);
    ~Matrix();

};

#endif
