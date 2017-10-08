#ifndef __OCR_H__
#define __OCR_H__

#include <algorithm>
#include <map>
#include <math.h>
#include <set>
#include <vector>
#include "fputils.h"

using namespace std;

class OCR {
    public:
        
        // Tipos
        using clave_t = char;
        using indice_t = unsigned int;
        using base_de_datos_t = map<clave_t, vector<indice_t>>;
        using elem_t = vector<double>;
        using datos_t = vector<elem_t>;
        
        // Constructores
        OCR(base_de_datos_t &bd, datos_t &datos);
        
        // Metodos de acceso a informacion
        const base_de_datos_t& base_de_datos() const;
        const datos_t& datos() const;
        
        // Metodos principales
        clave_t kNN(const unsigned int k, const elem_t &e) const;
        
        // Metodos auxiliares
        unsigned int filas(const datos_t &d) const;
        unsigned int filas(const elem_t &e) const;
        unsigned int columnas(const datos_t &d) const;
        unsigned int columnas(const elem_t &e) const;
        double distancia(const int i, const elem_t &e) const;
        double prod_interno(const elem_t &x, const elem_t &y) const;
        double norma2(const elem_t &e) const;
        void normalizar(elem_t &e) const;
        
        
    private:
        
        // Estructura de representacion
        base_de_datos_t _bd;
        datos_t _datos;
        datos_t _matriz_cov;
        unsigned int alpha;
        
        // Funciones auxiliares
        void _verificar_dimension(const elem_t &e) const;
        void _obtener_matriz_cov();
        pair<double, elem_t> _metodo_de_potencia(const datos_t &B, const elem_t &x) const;
        void _aplicar_cov(elem_t &x) const;
        void _deflacion(datos_t &A, const double a, const elem_t &p) const;
        void _obtener_cambio_de_base();
        elem_t _elem_random(const unsigned int n) const;
};


#endif //__OCR_H__