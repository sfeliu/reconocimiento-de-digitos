#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include "lib/ocr.h"

using namespace std;

int main() {
    
    // Lectura de base de datos de entrenamiento
    
    cout << "Leyendo base de datos de entrenamiento..." << endl;
    
    // Abro el archivo de datos de entrenamiento
    ifstream archivo_entrenamiento("data/train.csv");
    if (!archivo_entrenamiento.is_open()) throw runtime_error("No se pudo leer el archivo");
    archivo_entrenamiento.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto cabecera
    
    // Leo los datos
    OCR::base_de_datos_t bd;
    OCR::datos_t datos; // matriz de datos
    while (!archivo_entrenamiento.eof()) {
        // Obtengo la clave de la clase de la imagen
        OCR::clave_t clave;
        archivo_entrenamiento >> clave;
        // Agrego el numero de fila a la base de datos
        bd[clave].push_back((OCR::indice_t) datos.size());
        // Agrego fila a la matriz
        datos.push_back(OCR::dato_t());
        // Leo el vector
        while (archivo_entrenamiento.peek() == ',') {
            // Ignoro la coma
            archivo_entrenamiento.ignore();
            // Leo el valor del pixel
            unsigned int val;
            archivo_entrenamiento >> val;
            // Agrego el valor a la matriz
            datos.back().push_back(val);
        }
        archivo_entrenamiento.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto el salto de linea
        archivo_entrenamiento.peek(); // esto levanta el flag de EOF si se termino el archivo
    }
    
    OCR ocr(bd, datos, 1);
    cout <<ocr.reconocer(datos.back()) << endl;
    ocr.cant_componentes_PCA(10);
    cout <<ocr.reconocer(datos.back()) << endl;
    ocr.cant_componentes_PCA(5);
    ocr.cant_vecinos_kNN(20);
    cout <<ocr.reconocer(datos.back()) << endl;
    ocr.cant_componentes_PCA(10);
    cout <<ocr.reconocer(datos.back()) << endl;
    ocr.cant_componentes_PCA(11);
    cout <<ocr.reconocer(datos.back()) << endl;
       
    return 0;
}