#ifndef __MATRIZ_H__
#define __MATRIZ_H__

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <stdio.h>
#include "fputils.h"

using namespace std;

class Matriz {
    
    public:
        
        // Structs
        struct mascara {
            pair<unsigned int, unsigned int> fils;
            pair<unsigned int, unsigned int> cols;
        };
        
        // Constructores
        Matriz();
        Matriz(const Matriz &A);
        Matriz(const unsigned int n);
        Matriz(const unsigned int f, const unsigned int c);
        ~Matriz();
        Matriz& operator=(Matriz A);
        
        // Utilidades
        void swap(Matriz &A);
        bool vacia() const { return _matriz == NULL; };
        
        // Acceso de datos
        unsigned int filas() const;
        unsigned int columnas() const;
        unsigned int filas_real() const;
        unsigned int columnas_real() const;
        double& operator()(const unsigned int i);
        double operator()(const unsigned int i) const;
        double& operator()(const unsigned int i, const unsigned int j);
        double operator()(const unsigned int i, const unsigned int j) const;
        
        // Operadores aritmeticos
        Matriz operator+(const Matriz& A) const;
        Matriz operator-(const Matriz& A) const;
        Matriz operator*(const Matriz& A) const;
        Matriz operator*(const double c) const;
        Matriz operator/(const double c) const;
        Matriz producto_por_traspuesta() const;
        Matriz& operator+=(const Matriz&A) { *this = *this + A; return *this; };
        Matriz& operator-=(const Matriz&A) { *this = *this - A; return *this; };
        Matriz& operator*=(const Matriz&A) { *this = *this * A; return *this; };
        Matriz& operator*=(const double c) { *this = *this * c; return *this; };
        Matriz& operator/=(const double c) { *this = *this / c; return *this; };
        Matriz& multiplicar_por_traspuesta() { *this = producto_por_traspuesta(); return *this; };
        
        // Modificadores
        Matriz& trasponer();
        Matriz& enmascarar(pair<unsigned int, unsigned int> fils, pair<unsigned int, unsigned int> cols);
        Matriz& enmascarar_filas(unsigned int d, unsigned int h);
        Matriz& enmascarar_columnas(unsigned int d, unsigned int h);
        Matriz& desenmascarar();
        
        // Otros
        double producto_interno(const Matriz& A) const;
        double norma_euclidea() const;
        double distancia(const Matriz& A) const;
        Matriz normalizado() const;
        Matriz& normalizar();
        
        // Debugging
        void print() const {
            unsigned int fils = filas();
            unsigned int cols = columnas();
            for (unsigned int i = 0; i < fils; ++i) {
                for (unsigned int j = 0; j < cols; ++j) printf("%0.20f,", (*this)(i,j));
                printf("\n");
            }
        }
        
    private:
        
        // Estructura de representacion
        double** _matriz;
        unsigned int _fils;
        unsigned int _cols;
        mascara _mask;
        
        // Funciones auxiliares
        void _crear_matriz();
        void _verif_misma_dimension(const Matriz& A) const;
        void _verif_multiplicable(const Matriz& A) const;
    
};


// Definiciones

// Acceso de datos
inline unsigned int Matriz::filas() const {
    return _mask.fils.second - _mask.fils.first + 1;
};

inline unsigned int Matriz::columnas() const {
    return _mask.cols.second - _mask.cols.first + 1;
};

inline unsigned int Matriz::filas_real() const {
    return _fils;
};

inline unsigned int Matriz::columnas_real() const {
    return _cols;
};

inline double& Matriz::operator()(const unsigned int i) {
    return (*this)(i,0);
};

inline double Matriz::operator()(const unsigned int i) const {
    return (*this)(i,0);
};

inline double& Matriz::operator()(const unsigned int i, const unsigned int j) {
    return _matriz[i + _mask.fils.first][j + _mask.cols.first];
};

inline double Matriz::operator()(const unsigned int i, const unsigned int j) const {
    return _matriz[i + _mask.fils.first][j + _mask.cols.first];
};

// Modificadores

inline Matriz& Matriz::enmascarar(pair<unsigned int, unsigned int> fils, pair<unsigned int, unsigned int> cols) {
    enmascarar_filas(fils.first, fils.second);
    return enmascarar_columnas(cols.first, cols.second);
};

inline Matriz& Matriz::enmascarar_filas(unsigned int d, unsigned int h) {
    _mask.fils = make_pair(d, h-1);
    return *this;
};

inline Matriz& Matriz::enmascarar_columnas(unsigned int d, unsigned int h) {
    _mask.cols = make_pair(d, h-1);
    return *this;
};

inline Matriz& Matriz::desenmascarar() {
    return enmascarar(make_pair(0, _fils), make_pair(0, _cols));
};

// Otros

inline double Matriz::norma_euclidea() const {
    return sqrt(producto_interno(*this));
};

inline Matriz Matriz::normalizado() const {
    return *this / norma_euclidea();
};

inline Matriz& Matriz::normalizar() {
    *this = normalizado();
    return *this;
};


#endif //__MATRIZ_H__