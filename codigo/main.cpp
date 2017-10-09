#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include "lib/ocr.h"

using namespace std;

int main() {

    // Lectura de base de datos de entrenamiento

    //cout << "Leyendo base de datos de entrenamiento..." << endl;

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

    OCR ocr(bd, datos, 50, 15);
    /*
    // Abro el archivo de datos de testing
    ifstream archivo_test("data/test.csv");
    if (!archivo_test.is_open()) throw runtime_error("No se pudo leer el archivo");
    archivo_test.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto cabecera

    // Leo los datos
    OCR::datos_t datos_test; // matriz de datos
    while (!archivo_test.eof()) {
        // Agrego fila a la matriz
        datos_test.push_back(OCR::dato_t());
        // Obtengo el primer valor
        unsigned int primero;
        archivo_test >> primero;
        datos_test.back().push_back(primero);
        // Leo el vector
        while (archivo_test.peek() == ',') {
            // Ignoro la coma
            archivo_test.ignore();
            // Leo el valor del pixel
            unsigned int val;
            archivo_test >> val;
            // Agrego el valor a la matriz
            datos_test.back().push_back(val);
        }
        archivo_test.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto el salto de linea
        archivo_test.peek(); // esto levanta el flag de EOF si se termino el archivo
    }

    unsigned int n = datos_test.size();
    printf("ImageId,Label\n");
    for (unsigned int i = 0; i < n; ++i) {
        printf("%d,%c\n", i+1, ocr.reconocer(datos[i]));
    }*/

    return 0;
}