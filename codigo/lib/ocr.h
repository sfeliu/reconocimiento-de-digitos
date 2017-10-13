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
        using dato_t = vector<unsigned char>;
        using datos_t = vector<dato_t>;
        using base_de_datos_t = map<clave_t, datos_t>;
        
        // Utilidades
        static unsigned int cant_datos(const base_de_datos_t &bd);
        static unsigned int tam_datos(const base_de_datos_t &bd);

        // Constructores
        OCR(const base_de_datos_t &bd, unsigned int alpha = 0, unsigned int k = 0);

        // Metodos
        void datos_entrenamiento(const base_de_datos_t &bd);
        void alpha_PCA(const unsigned int alpha);
        unsigned int alpha_PCA() const;
        void k_KNN(const unsigned int k);
        unsigned int k_KNN() const;
        clave_t reconocer(const dato_t &dato) const;
        vector<clave_t> reconocer(const datos_t &datos) const;
        map<clave_t, vector<clave_t>> reconocer(const base_de_datos_t &bd) const;

    private:

        // Estructura de representacion
        base_de_datos_t _bd;
        unsigned int _cant_datos;
        unsigned int _tam_datos;
        unsigned int _alpha;
        unsigned int _k;
        Matriz _media;
        Matriz _muestra;
        Matriz _cov;
        Matriz _cb;

        // Funciones auxiliares
        void _aplicar_PCA();
        Matriz _normalizar_datos(const base_de_datos_t &bd) const;
        bool _metodo_de_la_potencia(const Matriz &B, const Matriz &x0, Matriz& v, double &a) const;
        void _aplicar_deflacion(Matriz &B, const Matriz &v, const double a) const;
        Matriz _vector_random(const unsigned int f) const;
        clave_t _KNN(const unsigned int k, const Matriz &A, const unsigned int col) const;
        double _distancia(const unsigned int j, const Matriz &A, const unsigned int col) const;

};


inline void OCR::alpha_PCA(const unsigned int alpha) {
    _alpha = alpha > _tam_datos ? 0 : alpha;
    _aplicar_PCA();
}

inline unsigned int OCR::alpha_PCA() const {
    return _alpha;
};

inline void OCR::k_KNN(const unsigned int k) {
    _k = (k == 0 || k > _cant_datos) ? _cant_datos : k;
};

inline unsigned int OCR::k_KNN() const {
    return _k;
};

inline OCR::clave_t OCR::reconocer(const dato_t &dato) const {
    return reconocer(datos_t(1, dato))[0];
};


#endif //__OCR_H__