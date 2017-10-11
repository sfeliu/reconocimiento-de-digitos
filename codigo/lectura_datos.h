#ifndef __LECTURA_DATOS_H__
#define __LECTURA_DATOS_H__

#include <fstream>
#include <iostream>
#include <limits>
#include "lib/ocr.h"

using namespace std;

void leer_datos_train(const char* ruta, OCR::base_de_datos_t &bd, OCR::datos_t &datos) {
    
    // Abro el archivo de datos de entrenamiento
    ifstream archivo(ruta);
    if (!archivo.is_open()) throw runtime_error("No se pudo leer el archivo");
    archivo.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto cabecera

    // Leo los datos
    bd = OCR::base_de_datos_t();
    datos = OCR::datos_t();
    while (!archivo.eof()) {
        // Obtengo la clave de la clase de la imagen
        OCR::clave_t clave;
        archivo >> clave;
        // Agrego el numero de fila a la base de datos
        bd[clave].push_back((OCR::indice_t) datos.size());
        // Agrego fila a la matriz
        datos.push_back(OCR::dato_t());
        // Leo el vector
        while (archivo.peek() == ',') {
            // Ignoro la coma
            archivo.ignore();
            // Leo el valor del pixel
            unsigned int val;
            archivo >> val;
            // Agrego el valor a la matriz
            datos.back().push_back(val);
        }
        archivo.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto el salto de linea
        archivo.peek(); // esto levanta el flag de EOF si se termino el archivo
    }
    
    archivo.close();
}

void leer_datos_test(const char* ruta, OCR::datos_t &datos) {
    
    // Abro el archivo de datos de testing
    ifstream archivo(ruta);
    if (!archivo.is_open()) throw runtime_error("No se pudo leer el archivo");
    archivo.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto cabecera

    // Leo los datos
    datos = OCR::datos_t();
    while (!archivo.eof()) {
        // Agrego fila a la matriz
        datos.push_back(OCR::dato_t());
        // Obtengo el primer valor
        unsigned int primero;
        archivo >> primero;
        datos.back().push_back(primero);
        // Leo el vector
        while (archivo.peek() == ',') {
            // Ignoro la coma
            archivo.ignore();
            // Leo el valor del pixel
            unsigned int val;
            archivo >> val;
            // Agrego el valor a la matriz
            datos.back().push_back(val);
        }
        archivo.ignore(numeric_limits<streamsize>::max(), '\n'); // descarto el salto de linea
        archivo.peek(); // esto levanta el flag de EOF si se termino el archivo
    }
    
    // Cierro el archivo
    archivo.close();
}

#endif //__LECTURA_DATOS_H__