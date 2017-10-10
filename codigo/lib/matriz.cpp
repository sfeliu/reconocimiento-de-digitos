#include "matriz.h"

// Constructores

Matriz::Matriz() : _matriz(NULL), _fils(0), _cols(0), _mask() {}

Matriz::Matriz(const Matriz &A) : _fils(A.filas()), _cols(A.columnas()) {
    _crear_matriz();
    for (unsigned int i = 0; i < _fils; ++i) {
        for (unsigned int j = 0; j < _cols; ++j)
            (*this)(i,j) = A(i,j);
    }
}

Matriz::Matriz(const unsigned int n) : _fils(n), _cols(n) {
    _crear_matriz();
}

Matriz::Matriz(const unsigned int f, const unsigned int c) : _fils(f), _cols(c) {
    _crear_matriz();
}

Matriz::~Matriz() {
    for (unsigned int i = 0; i < _fils; ++i)
        delete[] _matriz[i];
    delete[] _matriz;
}

Matriz& Matriz::operator=(Matriz A) {
    swap(A);
    return *this;
}


// Utilidades

void Matriz::swap(Matriz &A) {
    std::swap(_matriz, A._matriz);
    std::swap(_fils, A._fils);
    std::swap(_cols, A._cols);
    std::swap(_mask, A._mask);
}


// Operadores aritmeticos

Matriz Matriz::operator+(const Matriz& A) const {
    _verif_misma_dimension(A);
    unsigned int fils = filas();
    unsigned int cols = columnas();
    Matriz B(fils, cols);
    for (unsigned int i = 0; i < fils; ++i) {
        for (unsigned int j = 0; j < cols; ++j) B(i,j) = (*this)(i,j) + A(i,j);
    }
    return B;
}

Matriz Matriz::operator-(const Matriz& A) const {
    _verif_misma_dimension(A);
    unsigned int fils = filas();
    unsigned int cols = columnas();
    Matriz B(fils, cols);
    for (unsigned int i = 0; i < fils; ++i) {
        for (unsigned int j = 0; j < cols; ++j) B(i,j) = (*this)(i,j) - A(i,j);
    }
    return B;
}

Matriz Matriz::operator*(const Matriz& A) const {
    _verif_multiplicable(A);
    unsigned int fils = filas();
    unsigned int cols = A.columnas();
    unsigned int lon = columnas();
    Matriz B(fils, cols);
    for (unsigned int i = 0; i < fils; ++i) {
        for (unsigned int j = 0; j < cols; ++j) {
            double suma = 0;
            for (unsigned int k = 0; k < lon; ++k) suma += (*this)(i,k) * A(k,j);
            B(i,j) = suma;
        }
    }
    return B;
}

Matriz Matriz::operator*(const double c) const {
    unsigned int fils = filas();
    unsigned int cols = columnas();
    Matriz B(fils, cols);
    for (unsigned int i = 0; i < fils; ++i) {
        for (unsigned int j = 0; j < cols; ++j) B(i,j) = (*this)(i,j) * c;
    }
    return B;
}

Matriz Matriz::operator/(const double c) const {
    unsigned int fils = filas();
    unsigned int cols = columnas();
    Matriz B(fils, cols);
    for (unsigned int i = 0; i < fils; ++i) {
        for (unsigned int j = 0; j < cols; ++j) B(i,j) = (*this)(i,j) / c;
    }
    return B;
}

Matriz Matriz::producto_por_traspuesta() const {
    unsigned int fils = filas();
    unsigned int cols = columnas();
    Matriz A(fils, fils);
    for (unsigned int i = 0; i < fils; ++i) {
        for (unsigned int j = 0; j <= i; ++j) {
            double suma = 0;
            for (unsigned int k = 0; k < cols; ++k) suma += (*this)(i,k) * (*this)(j,k);
            A(i,j) = A(j,i) = suma;
        }
    }
    return A;
}


// Manipulacion matricial

Matriz& Matriz::trasponer() {
    unsigned int fils = filas();
    unsigned int cols = columnas();
    Matriz A(cols, fils);
    for (unsigned int i = 0; i < cols; ++i) {
        for (unsigned int j = 0; j < fils; ++j)
            A(i,j) = (*this)(j,i);
    }
    swap(A);
    return *this;
}


// Otros

double Matriz::producto_interno(const Matriz& v) const {
    unsigned int fils = filas();
    double prod = 0;
    for (unsigned int i = 0; i < fils; ++i) prod += (*this)(i) * v(i);
    return prod;
}

double Matriz::distancia(const Matriz& v) const {
    unsigned int fils = filas();
    double prod = 0;
    for (unsigned int i = 0; i < fils; ++i) prod += pow((*this)(i) - v(i), 2);
    return sqrt(prod);
}


// Funciones auxiliares

void Matriz::_crear_matriz() {
    _matriz = new double*[_fils];
    for (unsigned int i = 0; i < _fils; ++i)
        _matriz[i] = new double[_cols];
    _mask.fils.first = _mask.cols.first = 0;
    _mask.fils.second = _fils - 1;
    _mask.cols.second = _cols - 1;
}

void Matriz::_verif_misma_dimension(const Matriz& A) const {
    if (filas() != A.filas() || columnas() != A.columnas())
        throw runtime_error("Las dimensiones no son compatibles.");
}

void Matriz::_verif_multiplicable(const Matriz& A) const {
    if (columnas() != A.filas())
        throw runtime_error("Las dimensiones no son compatibles.");
}