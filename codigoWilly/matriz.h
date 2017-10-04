#ifndef __MATRIZ_H__
#define __MATRIZ_H__

#include "fputils.h"
#include <stdio.h>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <iostream>
using namespace std;

class Matriz {
    public:
        Matriz();
        Matriz(const Matriz &o); // constructor por copia
        Matriz(const int n); // matriz nula de nxn
        Matriz(const int f, const int c, const bool init = true); // matriz de fxn, inicializada como nula si init es true
        Matriz(const int f, const int c, const double a[], const int n); // construye una matriz de fxc a partir de un arreglo de tamanio n
        ~Matriz();

        static Matriz Identidad(const int n);

        void swap(Matriz &o);
        Matriz& operator=(Matriz o);
        double& operator()(const int i, const int j);
        const double& operator()(const int i, const int j) const;
        bool operator==(const Matriz &o) const;
        bool operator!=(const Matriz &o) const;
        Matriz& operator*(Matriz &o);
        Matriz& operator*(double c);
        Matriz& operator+(const Matriz &o) const;

        int filas() const;
        int columnas() const;
        bool esCuadrada() const;
        Matriz& invertir();
        Matriz& inversa() const;
        Matriz& trasponer();
        Matriz traspuesta() const;
        Matriz& multiplicarPorTraspuesta();
        Matriz& multiplicarPorTraspuestaBanda(const int p, const int q);
        double normaF() const;

        void eliminacionGaussiana();
        void eliminacionGaussiana(Matriz &b);
        void eliminacionGaussianaBanda(const int p, const int q);
        void eliminacionGaussianaBanda(const int p, const int q, Matriz &b);
        void factorizacionPLU(Matriz &P, Matriz &L, Matriz &U);
        Matriz& backwardSubstitution(Matriz &x, const Matriz &b);
        Matriz& forwardSubstitution(Matriz &x, const Matriz &b);
        Matriz& factorizacionCholesky(Matriz &L) const;
        Matriz& factorizacionCholeskyBanda(const int p, Matriz &L) const;
        void permutarFila(const int i, const int j);
        void multiplicarFilaPorEscalar(const int i, const double c);
        void restarMultiploDeFila(const int i, const int j, const double c);
        void restarMultiploDeFilaBanda(const int p, const int q, const int i, const int j, const double c);
        void dividirFilaPorEscalar(const int i, const double c);

        void print() const {
            for (int i = 0; i < filas(); ++i) {
                for (int j = 0; j < columnas(); ++j) {
                    printf("%.8f    ", (*this)(i,j));
                }
                printf("\n");
            }
        }

        double handle(int i, int j)const;
        Matriz& operator-(Matriz& o);
        Matriz& deflacion(double lambda, Matriz& B);
        Matriz& MatrizXVector(Matriz& o);
        Matriz& trasponerAux();
        Matriz& cargarDatos(vector<vector<vector<double> > > datos);
        Matriz& generarRandom();
        double norma(Matriz& x);
        Matriz& operator/(double div);
        double autoValor(Matriz& x, Matriz& B);
        vector<double> medias();
        Matriz& normalizar();
        Matriz& matrizDeCovarianza();
        pair<double,Matriz> metodoDeLasPotencias(int cantIter, Matriz& x);
        double norma2();

    private:
        int _filas, _columnas;
        double **_matriz;
        bool _traspuesta;
        int *_cols;

        void _crearMatriz(const int f, const int c, const bool init = true);
        void _verificarRango(const int f, const int c) const;
        void _verificarBanda(const int p, const int q) const;
        void _eliminacionGaussiana(Matriz *b);
        void _eliminacionGaussianaBanda(const int p, const int q, Matriz *b);
};

Matriz& operator*(double c, Matriz &m);




#endif //__MATRIZ_H__
