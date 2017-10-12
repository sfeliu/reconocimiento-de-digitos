#ifndef __OCR_H__
#define __OCR_H__

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <vector>
#include "matriz.h"

using namespace std;

class OCR {
    public:

        // Tipos
        using clave_t = char;
        using indice_t = unsigned int;
        using base_de_datos_t = map<clave_t, vector<indice_t>>;
        using dato_t = vector<unsigned char>;
        using datos_t = vector<dato_t>;

        // Constructores
        OCR(const base_de_datos_t &bd, const datos_t &datos, unsigned int alpha = 0, unsigned int k = 0);

        // Metodos
        inline const base_de_datos_t& base_de_datos() { return _bd; };
        inline void base_de_datos(const base_de_datos_t &bd) { _bd = bd; };
        void alpha_PCA(const unsigned int alpha);
        inline unsigned int alpha_PCA() const { return _alpha; };
        inline void k_KNN(const unsigned int k) { _k = k; };
        inline unsigned int k_KNN() const { return _k; };
        vector<clave_t> reconocer(const datos_t &datos) const;
        inline clave_t reconocer(const dato_t &dato) const { return reconocer(datos_t(1, dato))[0]; };

    private:

        // Estructura de representacion
        base_de_datos_t _bd;
        datos_t _datos;
        unsigned int _alpha;
        unsigned int _k;
        unsigned int _n;
        Matriz _media;
        Matriz _muestra;
        Matriz _cov;
        Matriz _cb;

        // Funciones auxiliares
        void _aplicar_PCA();
        Matriz _normalizar_datos(const datos_t &datos) const;
        bool _metodo_de_la_potencia(const Matriz &B, const Matriz &x0, Matriz& v, double &a) const;
        void _aplicar_deflacion(Matriz &B, const Matriz &v, const double a) const;
        Matriz _vector_random(const unsigned int f) const;
        clave_t _KNN(const unsigned int k, const Matriz &A, const unsigned int col) const;
        double _distancia(const unsigned int j, const Matriz &A, const unsigned int col) const;

};

#endif //__OCR_H__