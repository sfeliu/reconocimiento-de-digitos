#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <sstream>
#include "lectura_datos.h"
#include "lib/ocr.h"
#include "lib/tclap/CmdLine.h" // parser de argumentos

using namespace std;

void KFold(
    const unsigned int k_KFold,
    OCR &ocr,
    OCR::base_de_datos_t &bd_train,
    const OCR::datos_t &datos_train,
    const char* ruta_salida
    ) {
    
    // Obtengo el tamanio base de los folds y la cantidad de elementos restantes
    if (datos_train.size() == 0) throw runtime_error("No hay datos.");
    unsigned int tam_datos = datos_train[0].size();
    if (tam_datos == 0) throw runtime_error("No hay datos.");
    
    // Abro archivo de salida
    ofstream archivo_salida(ruta_salida);
    if (!archivo_salida.is_open()) throw runtime_error("No se puede escribir el archivo");
    archivo_salida << "iteracion,clase,prediccion" << endl;
    
    // Permuto los indices de la base de datos aleatoriamente
    for (auto it = bd_train.begin(); it != bd_train.end(); ++it)
        shuffle(it->second.begin(), it->second.end(), default_random_engine(time(NULL)));
    
    // Testeo con cada fold
    for (unsigned int k = 0; k < k_KFold; ++k) {
        
        OCR::base_de_datos_t bd_test;
        OCR::datos_t datos_test;
        
        // Obtengo fold de testing de cada clase
        for (auto it = bd_train.begin(); it != bd_train.end(); ++it) {
            
            vector<OCR::indice_t> &clase = it->second;
            
            // Determino tamanio del fold
            unsigned int cant_datos = clase.size();
            unsigned int restantes = cant_datos % k_KFold;
            unsigned int tam_fold = cant_datos / k_KFold + (k < restantes);
            
            // Saco fold para testing
            auto d = clase.begin();
            auto h = d + tam_fold;
            auto ins = bd_test.insert(make_pair(it->first, vector<OCR::indice_t>(d, h)));
            vector<OCR::indice_t> &indices_test = ins.first->second;
            clase.erase(d, h);
            
            // Obtengo datos de testing
            for (unsigned int i = 0; i < tam_fold; ++i) {
                datos_test.push_back(OCR::dato_t(tam_datos));
                for (unsigned int j = 0; j < tam_datos; ++j)
                    datos_test.back()[j] = datos_train[indices_test[i]][j];
            }
        }
    
        // Testeo OCR
        ocr.base_de_datos(bd_train);
        vector<OCR::clave_t> resultados = ocr.reconocer(datos_test);
        
        // Escribo resultados
        auto res_it = resultados.begin();
        for (auto it = bd_test.begin(); it != bd_test.end(); ++it) {
            
            OCR::clave_t clave = it->first;
            vector<OCR::indice_t> &clase = it->second;
            unsigned int cant_elem = clase.size();
            
            for (unsigned int i = 0; i < cant_elem; ++i) {
                archivo_salida << k << ',' << clave << ',' << *res_it << endl;
                ++res_it;
            }
        }
        
        // Restauro la base de entrenamiento
        for (auto it = bd_test.begin(); it != bd_test.end(); ++it) {
            
            auto &indices_test = it->second;
            auto &clase = bd_train[it->first];
            clase.insert(clase.end(), indices_test.begin(), indices_test.end());
        }
    }
    
    archivo_salida.close();
}

int main(int argc, const char *argv[]) {
    
    // Parseo argumentos
    TCLAP::CmdLine cmd("Hace cross validation del OCR", ' ', "");
    TCLAP::ValueArg<string> arg_train("t", "train", "Ruta del archivo de training", true, "", "string", cmd);
    TCLAP::ValueArg<string> arg_prefijo("p", "prefijo", "Prefijo de los archivos de salida", false, "cv", "string", cmd);
    TCLAP::ValueArg<unsigned int> arg_KNN_min("", "k-min", "Minimo valor de k-KNN", true, 0, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> arg_KNN_max("", "k-max", "Maximo valor de k-KNN", true, 0, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> arg_KFold_min("", "f-min", "Minimo valor de k-KFold", true, 0, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> arg_KFold_max("", "f-max", "Maximo valor de k-KFold", true, 0, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> arg_PCA_min("", "a-min", "Minimo valor de alpha-PCA", true, 0, "unsigned int", cmd);
    TCLAP::ValueArg<unsigned int> arg_PCA_max("", "a-max", "Maximo valor de alpha-PCA", true, 0, "unsigned int", cmd);
    cmd.parse(argc, argv);
    
    unsigned int KNN_min = arg_KNN_min.getValue();
    unsigned int KNN_max = arg_KNN_max.getValue();
    if (KNN_min > KNN_max) throw runtime_error("error: k-KNN: el minimo debe ser menor o igual al maximo.");
        
    unsigned int KFold_min = arg_KFold_min.getValue();
    unsigned int KFold_max = arg_KFold_max.getValue();
    if (KFold_min > KFold_max) throw runtime_error("error: k-KFold: el minimo debe ser menor o igual al maximo.");
    if (KFold_min <= 1) throw runtime_error("error: k-KFold: la cantidad de folds debe ser al menos 2.");
        
    unsigned int PCA_min = arg_PCA_min.getValue();
    unsigned int PCA_max = arg_PCA_max.getValue();
    if (PCA_min > PCA_max) throw runtime_error("error: alpha-PCA: el minimo debe ser menor o igual al maximo.");
        
    string prefijo = arg_prefijo.getValue();
    
    // Leo datos de entrenamiento
    OCR::base_de_datos_t bd_train;
    OCR::datos_t datos_train;
    leer_datos_train(arg_train.getValue().c_str(), bd_train, datos_train);
    
    // Inicializo OCR
    OCR::base_de_datos_t bd_vacia;
    OCR ocr(bd_vacia, datos_train, PCA_max, 0);
    
    // Hago cross validation
    for (unsigned int f = KFold_min; f <= KFold_max; ++f) {
        for (unsigned int a = PCA_max; a >= PCA_min; --a) {
            for (unsigned int k = KNN_min; k <= KNN_max; ++k) {
                
                printf("k-KFold = %u, alpha-PCA = %u, k-KNN = %u\n", f, a, k);
                
                // Defino ruta de salida
                ostringstream ss;
                ss << prefijo << "_f" << f << "_a" << a << "_k" << k << ".csv";
                string str = ss.str();
                char ruta_salida[str.length()];
                str.copy(ruta_salida, str.length());
                
                // Hago cross validation
                ocr.alpha_PCA(a);
                ocr.k_KNN(k);
                KFold(k, ocr, bd_train, datos_train, ruta_salida);
            }
        }
    }

    return 0;
}