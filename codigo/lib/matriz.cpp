#include "matriz.h"

using namespace std;
using namespace fputils;

void Matriz::_crearMatriz(const int f, const int c) {
    if (f <= 0 || c <= 0) {
        throw domain_error("Error: la cantidad de filas y columnas debe ser mayor a cero.");
    }
    _filas = f;
    _columnas = c;
    _traspuesta = false;
    _matriz = vector<vector<double>>(_filas, vector<double>(_columnas, 0));
}

Matriz::Matriz() : _filas(0), _columnas(0), _matriz(), _traspuesta(false) {}

Matriz::Matriz(const Matriz &o) {
    _crearMatriz(o.filas(), o.columnas());
    for (int i = 0; i < filas(); ++i) {
        for (int j = 0; j < columnas(); ++j) {
            (*this)(i,j) = o(i,j);
        }
    }
}

Matriz::Matriz(const int n) {
    _crearMatriz(n, n);
}

Matriz::Matriz(const int f, const int c) {
    _crearMatriz(f, c);
}

Matriz::Matriz(vector<vector<double>> &v) {
    _matriz.swap(v);
    _filas = (int) _matriz.size();
    _columnas = _filas == 0 ? 0 : (int) _matriz[0].size();
    _traspuesta = false;
}

Matriz Matriz::Identidad(const int n) {
    Matriz M(n);
    for (int i = 0; i < n; ++i) {
        M(i,i) = 1;
    }
    return M;
}

void Matriz::swap(Matriz &o) {
    std::swap(_filas, o._filas);
    std::swap(_columnas, o._columnas);
    std::swap(_traspuesta, o._traspuesta);
    _matriz.swap(o._matriz);
}

Matriz& Matriz::operator=(Matriz o) {
    swap(o);
    return *this;
}

double& Matriz::operator()(const int i, const int j) {
    _verificarRango(i,j);
    if (_traspuesta)
        return _matriz[j][i];
    else
        return _matriz[i][j];
}

const double& Matriz::operator()(const int i, const int j) const {
    _verificarRango(i,j);
    if (_traspuesta)
        return _matriz[j][i];
    else
        return _matriz[i][j];
}

bool Matriz::operator==(const Matriz &o) const {
    if (filas() != o.filas() || columnas() != o.columnas())
        return false;
    for (int i = 0; i < filas(); ++i) {
        for (int j = 0; j < columnas(); ++j) {
            if (!eq((*this)(i,j), o(i,j)))
                return false;
        }
    }
    return true;
}

bool Matriz::operator!=(const Matriz &o) const {
    return !(*this == o);
}

Matriz& Matriz::operator*(const Matriz &o) const {
    if (columnas() != o.filas()) {
        throw domain_error("Error: la multiplicación no esta definida para matrices de estas dimensiones");
    }
    Matriz *res = new Matriz(filas(), o.columnas());
    for (int i = 0; i < res->filas(); ++i) {
        for (int j = 0; j < res->columnas(); ++j) {
            (*res)(i,j) = 0;
            for (int k = 0; k < columnas(); ++k) {
                (*res)(i,j) += (*this)(i,k) * o(k,j);
            }
        }
    }
    return *res;
}

Matriz& Matriz::operator*(const double c) const {
    Matriz *res = new Matriz(filas(), columnas());
    for (int i = 0; i < res->filas(); ++i) {
        for (int j = 0; j < res->columnas(); ++j) {
            (*res)(i,j) = (*this)(i,j) * c;
        }
    }
    return *res;
}

Matriz& Matriz::operator+(const Matriz &o) const {
    if (columnas() != o.columnas() || filas() != o.filas()) {
        throw domain_error("Error: la suma no esta definida para matrices de estas dimensiones");
    }
    Matriz *res = new Matriz(filas(), columnas());
    for (int i = 0; i < res->filas(); ++i) {
        for (int j = 0; j < res->columnas(); ++j) {
            (*res)(i,j) = (*this)(i,j) + o(i,j);
        }
    }
    return *res;
}

int Matriz::filas() const {
    return _traspuesta ? _columnas : _filas;
}

int Matriz::columnas() const {
    return _traspuesta ? _filas : _columnas;
}

bool Matriz::esCuadrada() const {
    return _filas == _columnas;
}

Matriz& Matriz::trasponer() {
    _traspuesta = !_traspuesta;
    return *this;
}

Matriz Matriz::traspuesta() const {
    Matriz A(*this);
    return A.trasponer();
}

double Matriz::normaF() const {
    double suma = 0;
    for (int i = 0; i < filas(); ++i) {
        for (int j = 0; j < columnas(); ++j) {
            suma += pow((*this)(i,j), 2);
        }
    }
    return pow(suma, 0.5);
}

Matriz& Matriz::multiplicarPorTraspuesta() {
    Matriz A(filas());
    for (int i = 0; i < A.filas(); ++i) {
        for (int j = i; j < A.filas(); ++j) {
            for (int k = 0; k < columnas(); ++k)
                A(i,j) += (*this)(i,k) * (*this)(j,k);
            A(j,i) = A(i,j);
        }
    }
    swap(A);
    return *this;
}

void Matriz::_verificarRango(const int f, const int c) const {
    if (!(0 <= f && f < filas() && 0 <= c && c < columnas()))
        throw domain_error("Los indices de fila y/o columna estan fuera de rango.");
}

Matriz& operator*(const double c, const Matriz &m) {
    return m * c;
}