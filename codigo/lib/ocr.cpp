#include <iostream>
#include "ocr.h"

const unsigned int KNN_DEFAULT_K = 10;

// Constructores

OCR::OCR(base_de_datos_t &bd, datos_t &datos, unsigned int alpha, unsigned int k) : _bd(bd), _datos(datos), _alpha(alpha), _k(k) {

    unsigned int fils = datos.size();
    if (fils == 0) throw runtime_error("No hay datos.");
    unsigned int cols = datos[0].size();
    if (cols == 0) throw runtime_error("No hay datos.");

    // Obtengo la cantidad total de datos
    _n = fils;

    // Obtengo cantidad de vecinos para kNN
    if (_k == 0) _k = _n < KNN_DEFAULT_K ? _n : KNN_DEFAULT_K;

    // Obtengo la media de los datos
    _media = Matriz(cols, 1);
    for (unsigned int j = 0; j < cols; ++j) {
        double suma = 0;
        for (unsigned int i = 0; i < fils; ++i) suma += datos[i][j];
        _media(j) = suma / fils;
    }

    // Aplico PCA
    _aplicar_PCA();
}


// Metodos

vector<OCR::clave_t> OCR::reconocer(const datos_t &datos) const {
    Matriz A = _normalizar_datos(datos);
    A = _alpha == 0 ? A : _cb * A;
    unsigned int n = A.columnas();
    vector<clave_t> clases(n);
    for (unsigned int i = 0; i < n; ++i)
        clases[i] = _KNN(_k, A, i);
    return clases;
}

void OCR::alpha_PCA(const unsigned int alpha) {
    _alpha = alpha;
    // Si la cantidad de componentes es mayor a la que ya tengo, reaplico PCA.
    // Si no, dejo la cantidad de componentes necesarios.
    if (_alpha > _cb.filas_real()) {
        _aplicar_PCA();
    } else {
        _cb.enmascarar_filas(0, _alpha - 1);
        _muestra.enmascarar_filas(0, _alpha - 1);
    }
}


// Funciones auxiliares

void OCR::_aplicar_PCA() {
    //cout << "Aplicando PCA..." << endl;

    // Obtengo matriz de datos normalizados (los datos por columna)
    //cout << "  * Normalizando datos..." << endl;
    _muestra = _normalizar_datos(_datos);

    if (_alpha == 0) return;

    // Obtengo matriz de covarianza
    if (_cov.vacia()) {
        //cout << "  * Computando matriz de covarianza..." << endl;
        _cov = _muestra.producto_por_traspuesta();
    }

    // Obtengo matriz de cambio de base
    //cout << "  * Obteniendo matriz de cambio de base..." << endl;
    Matriz B = _cov;
    unsigned int cols = B.filas();
    _cb = Matriz(_alpha, cols);
    for (unsigned int k = 0; k < _alpha; ++k) {
        Matriz v = Matriz(); double a;
        while (!_metodo_de_la_potencia(B, _vector_random(cols), v, a)) {}
        for (unsigned int i = 0; i < cols; ++i) _cb(k,i) = v(i);
        _aplicar_deflacion(B, v, a);
    }

    // Aplico el cambio de base a los datos
    //cout << "  * Aplicando cambio de base..." << endl;
    _muestra = _cb * _muestra;
}

Matriz OCR::_normalizar_datos(const datos_t &datos) const {
    unsigned int fils = datos.size();
    unsigned int cols = datos[0].size();
    Matriz A = Matriz(cols, fils);
    double m = sqrt(_n - 1);
    for (unsigned int i = 0; i < cols; ++i) {
        for (unsigned int j = 0; j < fils; ++j)
            A(i,j) = (datos[j][i] - _media(i)) / m;
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
    // tuplas con distinta distancia.
    using tupla = pair<clave_t, double>;
    struct cmp {
        bool operator()(const tupla &a, const tupla &b) const { return fputils::lt(a.second, b.second); }
    };
    cmp menor;
    vector<tupla> cola;
    for (base_de_datos_t::const_iterator it = _bd.cbegin(); it != _bd.cend(); ++it) {
        unsigned int tam = it->second.size();
        for (unsigned int i = 0; i < tam; ++i) {
            tupla t = make_pair(it->first, _distancia(it->second[i], A, col));
            // Guardo t en la cola si hay menos de k elementos o si hay al menos k elementos
            // y la distancia de t es menor o igual que la del elemento con mayor distancia
            if (cola.size() < k || menor(t, cola[0])) {
                // Si la distancia de t es menor o igual que la del elemento con mayor distancia
                // saco al elemento con mayor distancia de la cola antes de insertar a t
                if (cola.size() >= k) {
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

double OCR::_distancia(const indice_t j, const Matriz &A, const unsigned int col) const {
    unsigned int n = A.filas();
    double suma = 0;
    for (unsigned int i = 0; i < n; ++i) suma += pow(_muestra(i,j) - A(i,col), 2);
    return suma; // no aplico la raiz para evitar introducir error de redondeo innecesario
}