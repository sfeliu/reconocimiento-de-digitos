#include "ocr.h"

using namespace std;

// Constructores

OCR::OCR(base_de_datos_t &bd, datos_t &datos) : _bd(), _datos(), _matriz_cov(), alpha(0) {
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
unsigned int OCR::filas(const datos_t &d) const {
    return d.size();
}

unsigned int OCR::filas(const elem_t &e) const {
    return e.size();
}

unsigned int OCR::columnas(const datos_t &d) const {
    return d.size() == 0 ? 0 : d[0].size();
}

unsigned int OCR::columnas(const elem_t &e) const {
    return 1;
}

double OCR::distancia(const int i, const elem_t &e) const {
    _verificar_dimension(e);
    
    double acum = 0;
    int dim = columnas(e);
    for (int j = 0; j < dim; ++j) {
        acum += pow(_datos[i][j] - e[j], 2);
    }
    return sqrt(acum);
}

double OCR::prod_interno(const elem_t &x, const elem_t &y) const {
    int dim = filas(x);
    double prod = 0;
    for (int k = 0; k < dim; ++k) prod += x[k] * y[k];
    return prod;
}

double OCR::norma2(const elem_t &e) const {
    return sqrt(prod_interno(e, e));
}

void OCR::normalizar(elem_t &e) const {
    int dim = filas(e);
    double norma = norma2(e);
    for (int k = 0; k < dim; ++k) e[k] /= norma;
}


// Funciones auxiliares

void OCR::_verificar_dimension(const elem_t &e) const {
    if (columnas(e) != columnas(_datos))
        throw runtime_error("La dimension del elemento no coincide con la de los datos.");
}

void OCR::_obtener_matriz_cov() {
    int c = columnas(_datos);
    int f = filas(_datos);

    // Obtengo medias
    vector<double> medias(c);
    for (int j = 0; j < c; ++j) {
        double suma = 0;
        for (int i = 0; i < f; ++i) {
            suma += _datos[i][j];
        }
        medias[j] = suma / f;
    }

    // Obtengo matriz traspuesta
    datos_t T(c, elem_t(f));
    for (int i = 0; i < c; ++i) {
        for (int j = 0; j < f; ++j) {
            T[i][j] = _datos[j][i] - medias[i];
        }
    }

    // Obtengo la matriz de covarianza
    _matriz_cov = datos_t(columnas(_datos), elem_t(columnas(_datos), 0));
    for (int i = 0; i < c; ++i) {
        for (int j = 0; j <= i; ++j) {

            double suma = 0;
            for (int k = 0; k < f; ++k) {
                suma += T[i][k] * T[j][k];
            }
            _matriz_cov[j][i] = _matriz_cov[i][j] = suma /= f-1;
        }
    }
    
}

pair<double, OCR::elem_t> OCR::_metodo_de_potencia(const datos_t &B, const elem_t &x) const {
    // Obtengo el autovector
    elem_t v = x;
    for (int k = 0; k < 300; ++k) {
        _aplicar_cov(v);
        normalizar(v);
    }
    
    // Obtengo el autovalor
    elem_t Bv = v;
    _aplicar_cov(Bv);
    double a = prod_interno(v, Bv) / prod_interno(v, v);
    
    return make_pair(a, v);
}

void OCR::_aplicar_cov(elem_t &x) const {
    int dim = filas(_matriz_cov);
    elem_t v(dim, 0);
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            v[i] += _matriz_cov[i][j] * x[j];
        }
    }
    x.swap(v);
}

void OCR::_deflacion(datos_t &A, const double a, const elem_t &e) const {
    int dim = filas(A);
    for (int i = 0; i < dim; ++i) {
        for (int j = i; j < dim; ++j) {
            double acum = 0;
            for (int k = 0; k < dim; ++k) acum += e[k] * e[k];
            A[j][i] = A[i][j] -= acum * a;
        }
    }
}

void OCR::_obtener_cambio_de_base() {
    int dim = filas(_matriz_cov);
    datos_t CB(dim, elem_t(alpha));
    datos_t B = _matriz_cov;
    for (unsigned int k = 0; k < alpha; ++k) {
        // Obtengo autovalor dominante y su autovector asociado
        pair<double, elem_t> p = _metodo_de_potencia(B, _elem_random(dim));
        // Pongo al vector como columna en CB
        for (int i = 0; i < dim; ++i) CB[i][k] = p.second[i];
        // Aplico deflacion
        _deflacion(B, p.first, p.second);
    }
}

OCR::elem_t OCR::_elem_random(const unsigned int n) const {
    elem_t e(n);
    for (unsigned int i = 0; i < n; ++i) e[i] = rand();
    normalizar(e);
    return e;
}