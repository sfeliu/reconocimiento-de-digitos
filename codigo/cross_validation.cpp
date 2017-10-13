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

char* obtener_ruta(const string prefijo, const unsigned int f, const unsigned int a, const unsigned int k) {
    ostringstream ss;
    ss << prefijo << "_f" << f << "_a" << a << "_k" << k << ".csv";
    string str = ss.str();
    unsigned int len = str.length();
    char *ruta_salida = new char[len + 1];
    str.copy(ruta_salida, len);
    ruta_salida[len] = '\0';
    return ruta_salida;
}

void KFold(const unsigned int k_KFold, OCR::base_de_datos_t &bd_train, const vector<unsigned int> &PCAs,
           const vector<unsigned int> &KNNs, const string prefijo) {
    
    if (OCR::cant_datos(bd_train) == 0 && OCR::tam_datos(bd_train) == 0) throw runtime_error("No hay datos.");
    
    unsigned int tam_PCAs = PCAs.size();
    unsigned int tam_KNNs = KNNs.size();
    
    // Creo archivos de salida
    for (unsigned int i = 0; i < tam_PCAs; ++i) {
        for (unsigned int j = 0; j < tam_KNNs; ++j) {
            
            char* ruta_salida = obtener_ruta(prefijo, k_KFold, PCAs[i], KNNs[j]);
            ofstream archivo_salida(ruta_salida);
            delete ruta_salida;
            if (!archivo_salida.is_open()) throw runtime_error("No se puede escribir el archivo");
            archivo_salida << "fold,clase,prediccion" << endl;
            archivo_salida.close();
        }
    }
    
    // Permuto los indices de la base de datos aleatoriamente
    for (auto it = bd_train.begin(); it != bd_train.end(); ++it)
        shuffle(it->second.begin(), it->second.end(), default_random_engine(time(NULL)));
    
    // Testeo con cada fold
    for (unsigned int k = 0; k < k_KFold; ++k) {
        
        OCR::base_de_datos_t bd_test;
        
        // Obtengo fold de testing de cada clase
        for (auto it = bd_train.begin(); it != bd_train.end(); ++it) {
            
            auto &datos_train = it->second;
            
            // Determino tamanio del fold
            unsigned int cant_datos = datos_train.size();
            unsigned int restantes = cant_datos % k_KFold;
            unsigned int tam_fold = cant_datos / k_KFold + (k < restantes);
            
            // Saco fold para testing
            auto desde = datos_train.begin();
            auto hasta = desde + tam_fold;
            bd_test.insert(make_pair(it->first, OCR::datos_t(desde, hasta)));
            datos_train.erase(desde, hasta);
        }
        
        // Testeo OCR
        OCR ocr(bd_train, PCAs[0], KNNs[0]);
        
        for (unsigned int i = 0; i < tam_PCAs; ++i) {    
            ocr.alpha_PCA(PCAs[i]);
            
            for (unsigned int j = 0; j < tam_KNNs; ++j) {
                ocr.k_KNN(KNNs[j]);
                
                map<OCR::clave_t, vector<OCR::clave_t>> res = ocr.reconocer(bd_test);
        
                // Escribo resultados
                for (auto it = res.begin(); it != res.end(); ++it) {
                    
                    // Abro el archivo
                    char* ruta_salida = obtener_ruta(prefijo, k_KFold, PCAs[i], KNNs[j]);
                    ofstream archivo_salida(ruta_salida, std::ios_base::app); // agrego, no sobreescribo
                    delete ruta_salida;
                    if (!archivo_salida.is_open()) throw runtime_error("No se puede escribir el archivo");
                    
                    // Escribo valores
                    OCR::clave_t clave = it->first;
                    auto &resultados = it->second;
                    unsigned int cant_elem = resultados.size();
                    
                    for (unsigned int i = 0; i < cant_elem; ++i)
                        archivo_salida << k << ',' << clave << ',' << resultados[i] << endl;
    
                    // Cierro el archivo
                    archivo_salida.close();
                }
            }
        }
        
        // Restauro la base de entrenamiento
        for (auto it = bd_test.begin(); it != bd_test.end(); ++it) {
            
            auto &datos_test = it->second;
            auto &datos_train = bd_train[it->first];
            datos_train.insert(datos_train.end(), datos_test.begin(), datos_test.end());
            datos_test.clear();
        }
    }
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
    leer_datos_train(arg_train.getValue().c_str(), bd_train);
    
    
    // Creo listas de parametros para el OCR
    
    unsigned int cant_KNNs = KNN_max - KNN_min + 1;
    vector<unsigned int> KNNs(cant_KNNs);
    for (unsigned int i = 0; i < cant_KNNs; ++i) KNNs[i] = KNN_min + i;
        
    unsigned int cant_PCAs = PCA_max - PCA_min + 1;
    vector<unsigned int> PCAs(cant_PCAs);
    for (unsigned int i = 0; i < cant_PCAs; ++i) PCAs[i] = PCA_max - i;
    
    
    // Hago cross validation
    for (unsigned int f = KFold_min; f <= KFold_max; ++f) {
        KFold(f, bd_train, PCAs, KNNs, prefijo);
    }

    return 0;
}