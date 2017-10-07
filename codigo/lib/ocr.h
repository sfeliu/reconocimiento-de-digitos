#ifndef __OCR_H__
#define __OCR_H__

#include <vector>
#include <map>
#include <set>
#include <math.h>
#include "matriz.h"

using namespace std;

class OCR {
    public:
        
        // Tipos
        using clave_t = char;
        using indice_t = unsigned int;
        using base_de_datos_t = map<clave_t, vector<indice_t>>;
        using datos_t = Matriz;
        using elem_t = Matriz;
        
        // Constructores
        OCR(base_de_datos_t &bd, vector<vector<double>> &datos);
        OCR(base_de_datos_t &bd, datos_t &datos);
        
        // Metodos de acceso a informacion
        const base_de_datos_t& base_de_datos() const;
        const datos_t& datos() const;
        
        // Metodos principales
        clave_t kNN(const unsigned int k, const elem_t &e) const;
        
        // Metodos auxiliares
        double distancia(const int i, const elem_t &e) const;
        
        
    private:
        
        // Estructura de representacion
        base_de_datos_t _bd;
        datos_t _datos;
        
        // Funciones auxiliares
        void _verificar_dimension(const elem_t &e) const;
};


#endif //__OCR_H__