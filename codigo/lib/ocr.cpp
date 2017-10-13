#include "ocr.h"

// Utilidades

unsigned int OCR::cant_datos(const base_de_datos_t &bd) {
    unsigned int cuenta = 0;
    for (auto it = bd.cbegin(); it != bd.cend(); ++it) cuenta += it->second.size();
    return cuenta;
}

unsigned int OCR::tam_datos(const base_de_datos_t &bd) {
    auto it = bd.cbegin();
    while (it != bd.cend() && it->second.size() == 0) ++it;
    return it == bd.cend() ? 0 : it->second[0].size();
}


// Constructores

OCR::OCR(const base_de_datos_t &bd, unsigned int alpha, unsigned int k) : _alpha(alpha), _k(k) {
    datos_entrenamiento(bd);
}


// Metodos

void OCR::datos_entrenamiento(const base_de_datos_t &bd) {
    
    // Obtengo la cantidad y el tamanio de datos
    _cant_datos = cant_datos(bd);
    _tam_datos = tam_datos(bd);
    if (_cant_datos == 0 || _tam_datos == 0) return;

    // Obtengo la media de los datos
    unsigned int fils = _cant_datos;
    unsigned int cols = _tam_datos;
    _media = Matriz(cols, 1);
    
    for (unsigned int j = 0; j < cols; ++j) {
        double suma = 0;
        for (auto it = bd.cbegin(); it != bd.cend(); ++it) {
            unsigned int tam = it->second.size();
            for (unsigned int k = 0; k < tam; ++k) suma += it->second[k][j];
        }
        _media(j) = suma / fils;
    }
    
    _bd = bd;
    k_KNN(_k);
    alpha_PCA(_alpha);
}

vector<OCR::clave_t> OCR::reconocer(const datos_t &datos) const {
    
    base_de_datos_t bd;
    datos_t &aux = bd[clave_t()];
    const datos_t *p = &datos;
    aux.swap(*const_cast<datos_t*>(p));
    
    auto res = reconocer(bd);
    
    aux.swap(*const_cast<datos_t*>(p));
    
    return res.begin()->second;
}

map<OCR::clave_t, vector<OCR::clave_t>> OCR::reconocer(const base_de_datos_t &bd) const {
    
    if (cant_datos(bd) == 0 || tam_datos(bd) == 0)
        throw runtime_error("No hay datos para reconocer.");
    
    Matriz A = _normalizar_datos(bd);
    A = _alpha == 0 ? A : _cb * A;
    
    map<clave_t, vector<clave_t>> res;
    unsigned int i = 0;
    
    for (auto it = bd.cbegin(); it != bd.cend(); ++it) {
        
        auto &clase = res[it->first];
        unsigned int tam = it->second.size();
        
        for (unsigned int k = 0; k < tam; ++k) {
            clase.push_back(_KNN(_k, A, i));
            ++i;
        }
    }
    
    return res;
}


// Funciones auxiliares

void OCR::_aplicar_PCA() {
    
    // Si la cantidad de componentes es menor o igual a la que ya tengo dejo
    // los componentes necesaria sin recalcular todo.
    if (!_cb.vacia() && _alpha <= _cb.filas_real()) {
        _cb.enmascarar_filas(0, _alpha);
        _muestra.enmascarar_filas(0, _alpha);
        return;
    }
    
    // Obtengo matriz de datos normalizados (los datos por columna)
    _muestra = _normalizar_datos(_bd);

    // Si alpha es cero no aplico PCA
    if (_alpha == 0) return;

    // Obtengo matriz de covarianza
    if (_cov.vacia()) _cov = _muestra.producto_por_traspuesta();

    // Obtengo matriz de cambio de base
    Matriz B = _cov;
    unsigned int cols = B.filas();
    _cb = Matriz(_alpha, cols);
    
    for (unsigned int k = 0; k < _alpha; ++k) {
        Matriz v; double a;
        while (!_metodo_de_la_potencia(B, _vector_random(cols), v, a)) {}
        for (unsigned int i = 0; i < cols; ++i) _cb(k,i) = v(i);
        _aplicar_deflacion(B, v, a);
    }

    // Aplico el cambio de base a los datos
    _muestra = _cb * _muestra;
}

Matriz OCR::_normalizar_datos(const base_de_datos_t &bd) const {
    
    unsigned int fils = cant_datos(bd);
    unsigned int cols = tam_datos(bd);
    
    Matriz A = Matriz(cols, fils);
    double m = sqrt(_cant_datos - 1);
    unsigned int i = 0; // indice de "fila" en bd
    
    for (auto it = bd.cbegin(); it != bd.cend(); ++it) {
        auto &datos = it->second;
        unsigned int tam = datos.size();
        
        for (unsigned int k = 0; k < tam; ++k) {
            for (unsigned int j = 0; j < cols; ++j) {
                A(j,i) = (datos[k][j] - _media(j)) / m;
            }
            ++i;
        }
    }
    
    return A;
}

bool OCR::_metodo_de_la_potencia(const Matriz& B, const Matriz &x0, Matriz& v, double &a) const {
    // Obtengo el autovector
    Matriz u = x0.normalizado();
    v = (B * u).normalizado();
    // Si x0 es autovector falla el metodo porque no puedo asegurar que su autovalor sea el dominante
    if (fputils::eq(v.distancia(u), 0)) return false;
    do {
        u = v;
        v = (B * v).normalizado();
    } while (v.distancia(u) > 10e-10); // Itero hasta que el cambio sea poco

    // Obtengo el autovalor
    a = v.producto_interno(B * v) / v.producto_interno(v);

    return true;
}

void OCR::_aplicar_deflacion(Matriz &B, const Matriz &v, const double a) const {
    B -= v.producto_por_traspuesta() * a;
}

Matriz OCR::_vector_random(const unsigned int f) const {
    srand(time(NULL));
    Matriz v(f, 1);
    for (unsigned int i = 0; i < f; ++i) v(i) = rand();
    return v;
}

OCR::clave_t OCR::_KNN(const unsigned int k, const Matriz &A, const unsigned int col) const {
    
    // Creo una cola de prioridad de tuplas <clave, distancia> en la cual la mayor prioridad
    // corresponde a la tupla de mayor distancia.
    // Cada tupla corresponde a un elemento de la base de datos de entrenamiento, donde clave
    // es la clave de su clase y distancia es su distancia con respecto a v.
    // En la cola se iran insertando las tuplas con menor distancia, guardando a lo sumo k
    // tuplas.
    
    using tupla = pair<clave_t, double>;
    struct cmp {
        bool operator()(const tupla &a, const tupla &b) const { return fputils::lt(a.second, b.second); }
    };
    cmp menor;
    vector<tupla> cola;
    
    unsigned int indice_dato = 0;
    for (auto it = _bd.cbegin(); it != _bd.cend(); ++it) {
        
        unsigned int tam = it->second.size();
        if (tam == 0) throw runtime_error("La base de datos tiene clases sin datos.");
        
        for (unsigned int i = 0; i < tam; ++i) {
            
            unsigned int tam_cola = cola.size();
            tupla t = make_pair(it->first, _distancia(indice_dato++, A, col));
            
            // Guardo t en la cola si hay menos de k elementos o si la distancia de t es menor
            // que la del elemento con mayor distancia
            if (tam_cola < k || menor(t, cola[0])) {
                // Si la distancia de t es menor que la del elemento con mayor distancia
                // saco al elemento con mayor distancia de la cola antes de insertar a t
                if (tam_cola >= k) {
                    pop_heap(cola.begin(), cola.end(), menor);
                    cola.pop_back();
                }
                cola.push_back(t);
                push_heap(cola.begin(), cola.end(), menor);
            }
        }
    }

    // Guardo por cada clave de la cola sus distancia asociadas.
    // El uso de multisets para guardar las distancia se explica mas abajo.
    using dict = map<clave_t, multiset<double>>;
    dict dists_por_clave;
    
    // Guardo en una lista iteradores a las claves con mas elementos
    vector<dict::iterator> claves_con_mas_vecinos;
    unsigned int max_cant_vecinos = 1;
    unsigned int tam_cola = cola.size();
    
    for (unsigned int i = 0; i < tam_cola; ++i) {
        // Agrego la distancia al multiset de la clave correspondiente,
        // definiendola antes si es necesario
        dict::iterator it = dists_por_clave.lower_bound(cola[i].first);
        if (it == dists_por_clave.end() || it->first != cola[i].first)
            it = dists_por_clave.insert(it, make_pair(cola[i].first, dict::mapped_type()));
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
    unsigned int tam = claves_con_mas_vecinos.size();
    for (unsigned int i = 1; i < tam; ++i) {
        if (lexicographical_compare(
                claves_con_mas_vecinos[i]->second.begin(), claves_con_mas_vecinos[i]->second.end(),
                clave_con_vecinos_mas_cercanos->second.begin(), clave_con_vecinos_mas_cercanos->second.end())
        )
            clave_con_vecinos_mas_cercanos = claves_con_mas_vecinos[i];
    }

    return clave_con_vecinos_mas_cercanos->first;
}

double OCR::_distancia(const unsigned int j, const Matriz &A, const unsigned int col) const {
    unsigned int n = A.filas();
    double suma = 0;
    for (unsigned int i = 0; i < n; ++i) suma += pow(_muestra(i,j) - A(i,col), 2);
    return suma; // no aplico la raiz para evitar introducir error de redondeo innecesario
}