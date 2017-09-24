#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

using namespace std;
using ocr_clave = char;
using ocr_elem = vector<int>;
using ocr_clase = vector<ocr_elem>;
using ocr_bd = map<ocr_clave, ocr_clase>;

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