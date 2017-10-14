#include <fstream>
#include <iostream>
#include <vector>
#include "lectura_datos.h"
#include "lib/ocr.h"
#include "lib/tclap/CmdLine.h"

using namespace std;

int main(int argc, const char *argv[]) {
    
    // Parseo argumentos
    TCLAP::CmdLine cmd("Reconoce caracteres manuscritos", ' ', "");
    TCLAP::ValueArg<string> arg_test("q", "test-set", "Ruta del archivo de testing", true, "", "string", cmd);
    TCLAP::ValueArg<string> arg_output("o", "output", "Ruta del archivo de salida", false, "resultados.csv", "string", cmd);
    TCLAP::ValueArg<unsigned int> arg_k_KNN("k", "kNN", "k-KNN", false, 0, "unsigned int", cmd);
    TCLAP::ValueArg<string> arg_train("i", "train-set", "Ruta del archivo de entrenamiento", false, "", "string", cmd);
    TCLAP::ValueArg<unsigned int> arg_alpha_PCA("a", "alpha", "alpha-PCA", true, 0, "unsigned int", cmd);
    cmd.parse(argc, argv);
    
    
    // Abro archivo de salida
    ofstream archivo_resultados(arg_output.getValue());
    if (!archivo_resultados.is_open()) throw runtime_error("No se puede escribir el archivo de salida.");
    

    // Lectura de base de datos de entrenamiento
    OCR::base_de_datos_t bd_train;
    leer_datos_train(arg_train.getValue().c_str(), bd_train);
    
    
    // Lectura de datos de testing
    OCR::datos_t datos_test;
    leer_datos_test(arg_test.getValue().c_str(), datos_test);
    
    
    // Reconozco caracteres
    unsigned int alpha_PCA = arg_alpha_PCA.getValue();
    unsigned int k_KNN = arg_k_KNN.getValue();
    
    OCR ocr(bd_train, alpha_PCA, k_KNN);
    vector<OCR::clave_t> resultados = ocr.reconocer(datos_test);
    
    
    // Escribo resultados
    archivo_resultados << "ImageId,Label" << endl;
    
    unsigned int tam = resultados.size();   
    for (unsigned int i = 0; i < tam; ++i) {
        archivo_resultados << i+1 << ',' << resultados[i] << endl;
    }
    
    archivo_resultados.close();
    
    
    return 0;
}