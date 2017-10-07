#ifndef __MATRIZ_H__
#define __MATRIZ_H__

#include "fputils.h"
#include <vector>
#include <stdio.h>
#include <algorithm>
#include <cmath>
#include <stdexcept>

class Matriz {
    public:
        Matriz();
        Matriz(const Matriz &o); // constructor por copia
        Matriz(const int n); // matriz nula de nxn
        Matriz(const int f, const int c); // matriz nula de fxn
        Matriz(vector<vector<double>> &v); // construye matriz a partir de vector de vectores por referencia

        static Matriz Identidad(const int n);

        void swap(Matriz &o);
        
        Matriz& operator=(Matriz o); // operador de asignacion
        double& operator()(const int i, const int j);
        const double& operator()(const int i, const int j) const;
        bool operator==(const Matriz &o) const;
        bool operator!=(const Matriz &o) const;
        Matriz& operator*(const Matriz &o) const;
        Matriz& operator*(const double c) const;
        Matriz& operator+(const Matriz &o) const;

        int filas() const;
        int columnas() const;
        bool esCuadrada() const;
        Matriz& trasponer();
        Matriz traspuesta() const;
        Matriz& multiplicarPorTraspuesta();
        double normaF() const;

        void print() const {
            for (int i = 0; i < filas(); ++i) {
                for (int j = 0; j < columnas(); ++j) {
                    printf("%.8f    ", (*this)(i,j));
                }
                printf("\n");
            }
        }

    private:
        int _filas, _columnas;
        vector<vector<double>> _matriz;
        bool _traspuesta;
        vector<int> _cols;

        void _crearMatriz(const int f, const int c);
        void _verificarRango(const int f, const int c) const;
};

Matriz& operator*(const double c, const Matriz &m);

#endif //__MATRIZ_H__