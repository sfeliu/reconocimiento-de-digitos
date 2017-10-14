#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <random>
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
            unsigned int alpha_PCA = PCAs[i];
            ocr.alpha_PCA(alpha_PCA);

            for (unsigned int j = 0; j < tam_KNNs; ++j) {
                unsigned int k_KNN = KNNs[j];
                ocr.k_KNN(k_KNN);

                map<OCR::clave_t, vector<OCR::clave_t>> res = ocr.reconocer(bd_test);

                // Escribo resultados
                for (auto it = res.begin(); it != res.end(); ++it) {
                    
                    // Abro el archivo
                    char* ruta_salida = obtener_ruta(prefijo, k_KFold, alpha_PCA, k_KNN);
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

vector<unsigned int> leer_parametros(const char* ruta) {
    ifstream archivo(ruta);
    if (!archivo.is_open()) throw runtime_error("No se pudo abrir el archivo.");

    vector<unsigned int> paramas;
    while (isdigit(archivo.peek())) {
        unsigned int val;
        archivo >> val;
        paramas.push_back(val);
        archivo.ignore();
    }

    archivo.close();

    return paramas;
}

int main(int argc, const char *argv[]) {

    // Parseo argumentos

    TCLAP::CmdLine cmd("Hace cross validation del OCR", ' ', "");
    TCLAP::ValueArg<string> arg_train("t", "train", "Ruta del archivo de training", true, "", "string", cmd);
    TCLAP::ValueArg<string> arg_prefijo("p", "prefijo", "Prefijo de los archivos de salida", false, "cv", "string", cmd);
    TCLAP::ValueArg<string> arg_KNNs("k", "knns", "Ruta del archivo de k's de KNN", true, "cv", "string", cmd);
    TCLAP::ValueArg<string> arg_KFolds("f", "folds", "Ruta del archivo de folds de KFold", true, "cv", "string", cmd);
    TCLAP::ValueArg<string> arg_PCAs("a", "alphas", "Ruta del archivo de alphas de PCA", true, "cv", "string", cmd);
    cmd.parse(argc, argv);

    string ruta_KFolds = arg_KFolds.getValue();
    string ruta_PCAs = arg_PCAs.getValue();
    string ruta_KNNs = arg_KNNs.getValue();
    string prefijo = arg_prefijo.getValue();


    // Leo datos de entrenamiento
    OCR::base_de_datos_t bd_train;
    leer_datos_train(arg_train.getValue().c_str(), bd_train);


    // Creo listas de parametros para el OCR
    vector<unsigned int> KFolds = leer_parametros(ruta_KFolds.c_str());
    vector<unsigned int> PCAs = leer_parametros(ruta_PCAs.c_str());
    vector<unsigned int> KNNs = leer_parametros(ruta_KNNs.c_str());
    
    
    // Hago cross validation
    for (auto it = KFolds.begin(); it != KFolds.end(); ++it) {
        KFold(*it, bd_train, PCAs, KNNs, prefijo);
    }

    return 0;
}