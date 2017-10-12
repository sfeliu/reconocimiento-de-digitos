#include <fstream>
#include <iostream>
#include <vector>
#include "lectura_datos.h"
#include "lib/ocr.h"
#include "lib/timer.h"
#include "lib/tclap/CmdLine.h"

using namespace std;

int main(int argc, const char *argv[]) {
    
    // Parseo argumentos
    TCLAP::CmdLine cmd("Hace cross validation del OCR", ' ', "");
    TCLAP::ValueArg<string> arg_test("t", "testing", "Ruta del archivo de testing", true, "", "string", cmd);
    TCLAP::ValueArg<string> arg_train("e", "entrenamiento", "Ruta del archivo de entrenamiento", true, "", "string", cmd);
    TCLAP::ValueArg<string> arg_output("o", "output", "Ruta del archivo de salida", false, "resultados.csv", "string", cmd);
    TCLAP::ValueArg<unsigned int> arg_k_KNN("k", "kNN", "k-KNN", true, 0, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> arg_alpha_PCA("a", "alpha", "alpha-PCA", true, 0, "unsigned int", cmd);
    cmd.parse(argc, argv);
    
    
    // Inicializo timer
    
    Timer timer = Timer();


    // Lectura de base de datos de entrenamiento

    cout << "Leyendo base de datos de entrenamiento... " << flush;
    timer.iniciar();

    OCR::base_de_datos_t bd_train;
    OCR::datos_t datos_train;
    leer_datos_train(arg_train.getValue().c_str(), bd_train, datos_train);
    
    cout << timer.tiempo() << "s" << endl;
    
    
    // Lectura de datos de testing

    cout << "Leyendo datos de testing... " << flush;
    timer.iniciar();

    OCR::datos_t datos_test;
    leer_datos_test(arg_test.getValue().c_str(), datos_test);
    
    cout << timer.tiempo() << "s" << endl;
    

    // Reconocimiento de caracter
    
    cout << "Inicializando OCR... " << flush;
    timer.iniciar();
    
    // Inicializo OCR
    OCR ocr(bd_train, datos_train, arg_alpha_PCA.getValue(), arg_k_KNN.getValue());
    
    cout << timer.tiempo() << "s" << endl;
    
    cout << "Reconociendo caracteres... " << flush;
    timer.iniciar();
    
    vector<OCR::clave_t> resultados = ocr.reconocer(datos_test);
    
    cout << timer.tiempo() << "s" << endl;
    
    
    // Escritura de resultados
    
    cout << "Escribiendo resultados... " << flush;
    timer.iniciar();
    
    ofstream archivo_resultados(arg_output.getValue());
    archivo_resultados << "ImageId,Label" << endl;

    unsigned int n = resultados.size();
    for (unsigned int i = 0; i < n; ++i) {
        archivo_resultados << i+1 << ',' << resultados[i] << endl;
    }
    
    archivo_resultados.close();
    
    cout << timer.tiempo() << "s" << endl;


    return 0;
}