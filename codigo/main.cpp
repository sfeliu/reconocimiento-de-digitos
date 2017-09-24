#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <vector>

using namespace std;
using ocr_clave = char;
using ocr_elem = vector<int>;
using ocr_clase = vector<ocr_elem>;
using ocr_bd = map<ocr_clave, ocr_clase>;

unsigned long dist_cuad(const ocr_elem &x, const ocr_elem &y) {
    if (x.size() != y.size() || x.size() == 0)
        throw runtime_error("Los vectores deben tener la misma dimension (no nula).");
    
    unsigned long res = 0;
    for (unsigned int i = 0; i < x.size(); ++i) res += (x[i] - y[i]) * (x[i] - y[i]);
    return res;
}

ocr_clave kNN(const unsigned int k, const ocr_bd &bd, const ocr_elem &e) {
    if (k <= 0) throw runtime_error("k debe ser mayor a 0.");
    
    // Creo una cola de prioridad de tuplas <clave, dist_cuad> en la cual la mayor prioridad
    // corresponde a la tupla de mayor dist_cuad.
    // Cada tupla corresponde a un elemento de la base de datos de entrenamiento, donde clave
    // es la clave de su clase y dist_cuad es su distancia con respecto a e al cuadrado.
    // En la cola se iran insertando las tuplas con menor dist_cuad, guardando a lo sumo k
    // tuplas con distinta dist_cuad.
    using tupla = pair<ocr_clave, unsigned long>;
    struct menor {
        bool operator()(const tupla &a, const tupla &b) const { return a.second < b.second; }
    };
    vector<tupla> cola;
    for (ocr_bd::const_iterator it = bd.cbegin(); it != bd.cend(); ++it) {
        for (unsigned int i = 0; i < it->second.size(); ++i) {
            tupla t = make_pair(it->first, dist_cuad(it->second[i], e));
            // Guardo t en la cola si hay menos de k elementos o si hay al menos k elementos
            // y la dist_cuad de t es menor o igual que la del elemento con mayor dist_cuad
            if (cola.size() < k || !menor()(cola[0], t)) {
                // Si la dist_cuad de t es menor o igual que la del elemento con mayor dist_cuad
                // saco al elemento con mayor dist_cuad de la cola antes de insertar a t
                if (cola.size() >= k) pop_heap(cola.begin(), cola.end(), menor());
                cola.push_back(t);
                push_heap(cola.begin(), cola.end(), menor());
            }
        }
    }
    
    // Guardo por cada clave de la cola sus dist_cuad asociadas.
    // El uso de multisets para guardar las dist_cuad se explica mas abajo.
    using dict = map<ocr_clave, multiset<unsigned long>>;
    dict dists_por_clave;
    // Guardo en una lista iteradores a las claves con mas elementos
    vector<dict::iterator> claves_con_mas_vecinos;
    unsigned int max_cant_vecinos = 0;
    for (unsigned int i = 0; i < cola.size(); ++i) {
        // Agrego la dist_cuad al multiset de la clave correspondiente,
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

int main() {
    
    // Lectura de base de datos de entrenamiento
    
    // Abro el archivo de datos de entrenamiento
    ifstream datos_entrenamiento("data/train.csv");
    if (!datos_entrenamiento.is_open()) throw runtime_error("No se pudo leer el archivo");
    datos_entrenamiento.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto cabecera
    
    // Leo los datos
    ocr_bd bd_entrenamiento;
    while (!datos_entrenamiento.eof()) {
        // Obtengo la clave de la clase de la imagen
        ocr_clave clave;
        datos_entrenamiento >> clave;
        // Agrego un vector vacio a la clase
        bd_entrenamiento[clave].push_back(ocr_elem());
        // Obtengo las coordenadas del vector
        while (datos_entrenamiento.peek() == ',') {
            // Ignoro la coma
            datos_entrenamiento.ignore();
            // Leo el valor del pixel
            int val;
            datos_entrenamiento >> val;
            // Agrego el valor al vector correspondiente
            bd_entrenamiento[clave].back().push_back(val);
        }
        datos_entrenamiento.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto el salto de linea
        datos_entrenamiento.peek(); // esto levanta el flag de EOF si se termino el archivo
    }
    
    return 0;
}