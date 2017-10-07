#include "ocr.h"

using namespace std;

// Constructores

OCR::OCR(base_de_datos_t &bd, vector<vector<double>> &datos) : _bd(), _datos(datos), _matriz_cov() {
    _bd.swap(bd);
    _obtener_matriz_cov();
}

OCR::OCR(base_de_datos_t &bd, datos_t &datos) : _bd(), _datos(), _matriz_cov() {
    _bd.swap(bd);
    _datos.swap(datos);
    _obtener_matriz_cov();
}
        
        
// Metodos de acceso a informacion

const OCR::base_de_datos_t& OCR::base_de_datos() const {
    return _bd;
}

const OCR::datos_t& OCR::datos() const {
    return _datos;
}


// Métodos principales

OCR::clave_t OCR::kNN(const unsigned int k, const elem_t &e) const {
    
    if (k <= 0) throw runtime_error("k debe ser mayor a 0.");
    _verificar_dimension(e);
    
    // Creo una cola de prioridad de tuplas <clave, distancia> en la cual la mayor prioridad
    // corresponde a la tupla de mayor distancia.
    // Cada tupla corresponde a un elemento de la base de datos de entrenamiento, donde clave
    // es la clave de su clase y distancia es su distancia con respecto a e.
    // En la cola se iran insertando las tuplas con menor distancia, guardando a lo sumo k
    // tuplas con distinta distancia.
    using tupla = pair<clave_t, double>;
    struct menor {
        bool operator()(const tupla &a, const tupla &b) const { return fputils::lt(a.second, b.second); }
    };
    vector<tupla> cola;
    for (base_de_datos_t::const_iterator it = _bd.cbegin(); it != _bd.cend(); ++it) {
        for (unsigned int i = 0; i < it->second.size(); ++i) {
            tupla t = make_pair(it->first, distancia(it->second[i], e));
            // Guardo t en la cola si hay menos de k elementos o si hay al menos k elementos
            // y la distancia de t es menor o igual que la del elemento con mayor distancia
            if (cola.size() < k || !menor()(cola[0], t)) {
                // Si la distancia de t es menor o igual que la del elemento con mayor distancia
                // saco al elemento con mayor distancia de la cola antes de insertar a t
                if (cola.size() >= k) {
                    pop_heap(cola.begin(), cola.end(), menor());
                    cola.pop_back();
                }
                cola.push_back(t);
                push_heap(cola.begin(), cola.end(), menor());
            }
        }
    }
    
    // Guardo por cada clave de la cola sus distancia asociadas.
    // El uso de multisets para guardar las distancia se explica mas abajo.
    using dict = map<clave_t, multiset<double>>;
    dict dists_por_clave;
    // Guardo en una lista iteradores a las claves con mas elementos
    vector<dict::iterator> claves_con_mas_vecinos;
    unsigned int max_cant_vecinos = 0;
    for (unsigned int i = 0; i < cola.size(); ++i) {
        // Agrego la distancia al multiset de la clave correspondiente,
        // definiendola antes si es necesario
        dict::iterator it = dists_por_clave.lower_bound(cola[i].first);
        if (it->first != cola[i].first) it = dists_por_clave.insert(it, make_pair(cola[i].first, dict::mapped_type()));
        it->second.insert(cola[i].second);
        
        // Actualizo la lista de claves con mas vecinos y la maxima cantidad de vecinos
        if (it->second.size() >= max_cant_vecinos) {
            // Si la cantidad de vecinos de la clave actual supera la maxima cantidad de vecinos
            // elimino todas las claves de la lista antes de insertar la clave actual
            if (it->second.size() > max_cant_vecinos) {
                claves_con_mas_vecinos.clear();
                max_cant_vecinos = it->second.size();
            }
            claves_con_mas_vecinos.push_back(it);
        }
    }
    
    // De todas las claves con la cantidad mayor de elementos obtengo aquella que tenga a un
    // elemento a menor distancia que los elementos de las demas claves.
    // Esto es, si consediramos las distancias asociadas a cada clave en listas ordenadas, la
    // menor lista lexicograficamente.
    // Dado que los multisets guardan a sus elementos de de menor a mayor, puedo hacer obtener
    // esto haciendo uso de la funcion lexicographical_compare
    dict::iterator clave_con_vecinos_mas_cercanos = claves_con_mas_vecinos[0];
    for (unsigned int i = 1; i < claves_con_mas_vecinos.size(); ++i) {
        if (lexicographical_compare(
                claves_con_mas_vecinos[i]->second.begin(), claves_con_mas_vecinos[i]->second.end(),
                clave_con_vecinos_mas_cercanos->second.begin(), clave_con_vecinos_mas_cercanos->second.end())
        )
            clave_con_vecinos_mas_cercanos = claves_con_mas_vecinos[i];
    }
    
    return clave_con_vecinos_mas_cercanos->first;
}


// Metodos auxiliares

double OCR::distancia(const int i, const elem_t &e) const {
    _verificar_dimension(e);
    
    double acum = 0;
    for (int j = 0; j < e.columnas(); ++j) {
        acum += pow(_datos(i,j) - e(0,j), 2);
    }
    return sqrt(acum);
}


// Funciones auxiliares

void OCR::_verificar_dimension(const elem_t &e) const {
    if (e.columnas() != _datos.columnas())
        throw runtime_error("La dimension del elemento no coincide con la de los datos.");
}

void OCR::_obtener_matriz_cov() {
    printf("obteniendo medias\n");
    // Obtengo medias
    vector<double> medias(_datos.columnas());
    for (int j = 0; j < _datos.columnas(); ++j) {
        double suma = 0;
        for (int i = 0; i < _datos.filas(); ++i) {
            suma += _datos(i,j);
        }
        medias[j] = suma / _datos.filas();
    }
    
    printf("obteniendo matriz\n");
    // Obtengo la matriz de covarianza
    _matriz_cov = Matriz(_datos.columnas());
    for (int i = 0; i < _matriz_cov.filas(); ++i) {
        for (int j = i; j < _matriz_cov.filas(); ++j) {
            
            for (int k = 0; k < _datos.filas(); ++k) {
                _matriz_cov(i,j) = (_datos(k,i) - medias[i]) * (_datos(k,j) - medias[j]) / (_datos.filas()-1);
            }
            _matriz_cov(j,i) = _matriz_cov(i,j);
            
        }
    }
    
}