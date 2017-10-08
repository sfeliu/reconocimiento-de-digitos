#ifndef __MATRIZ_H__
#define __MATRIZ_H__

#include <algorithm>
#include <stdexcept>
#include <stdio.h>
#include "fputils.h"
#include "math.h"

using namespace std;

class Matriz {
    
    public:
        
        // Constructores
        Matriz();
        Matriz(const Matriz &A);
        Matriz(const unsigned int n);
        Matriz(const unsigned int f, const unsigned int c);
        ~Matriz();
        Matriz& operator=(Matriz A);
        
        // Utilidades
        void swap(Matriz &A);
        inline bool vacia() const { return _matriz == NULL; };
        
        // Acceso de datos
        inline unsigned int filas() const { return _fils; };
        inline unsigned int columnas() const { return _cols; };
        inline double& operator()(const unsigned int i) { return _matriz[i][0]; };
        inline double operator()(const unsigned int i) const { return _matriz[i][0]; };
        inline double& operator()(const unsigned int i, const unsigned int j) { return _matriz[i][j]; };
        inline double operator()(const unsigned int i, const unsigned int j) const { return _matriz[i][j]; };
        
        // Operadores aritmeticos
        Matriz operator+(const Matriz& A) const;
        Matriz operator-(const Matriz& A) const;
        Matriz operator*(const Matriz& A) const;
        Matriz operator*(const double c) const;
        Matriz operator/(const double c) const;
        Matriz producto_por_traspuesta() const;
        inline Matriz& operator+=(const Matriz&A) { *this = *this + A; return *this; };
        inline Matriz& operator-=(const Matriz&A) { *this = *this - A; return *this; };
        inline Matriz& operator*=(const Matriz&A) { *this = *this * A; return *this; };
        inline Matriz& operator*=(const double c) { *this = *this * c; return *this; };
        inline Matriz& operator/=(const double c) { *this = *this / c; return *this; };
        inline Matriz& multiplicar_por_traspuesta() { *this = producto_por_traspuesta(); return *this; };
        
        // Manipulacion matricial
        Matriz& trasponer();
        
        // Otros
        double producto_interno(const Matriz& A) const;
        inline double norma_euclidea() const { return sqrt(producto_interno(*this)); };
        double distancia(const Matriz& A) const;
        inline Matriz normalizado() const { return *this / norma_euclidea(); };
        inline Matriz& normalizar() { *this = normalizado(); return *this; };
        
        // Debugging
        void print() const {
            unsigned int fils = filas();
            unsigned int cols = columnas();
            for (unsigned int i = 0; i < fils; ++i) {
                for (unsigned int j = 0; j < cols; ++j) printf("%f, ", (*this)(i,j));
                printf("\n");
            }
        }
        
    private:
        
        // Estructura de representacion
        double** _matriz;
        unsigned int _fils;
        unsigned int _cols;
        
        // Funciones auxiliares
        void _crear_matriz();
        void _verif_misma_dimension(const Matriz& A) const;
        void _verif_multiplicable(const Matriz& A) const;
    
};

#endif //__MATRIZ_H__