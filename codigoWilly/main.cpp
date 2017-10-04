#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <vector>
#include <algorithm>
#include <fstream>
#include <limits>
#include "matriz.h"
using namespace std;
typedef vector< vector< vector< double > > > archivo;

int reversed(int a ){
    int reverse = 0;
    while(a != 0){
        reverse = reverse * 10; 
        reverse = reverse + a % 10;
        a = a / 10; 
    }
    return reverse;
}

int mcd(int a, int b){
    while(b > 0)
    {
        int temp = b;
        b = a%b;
        a = temp;
    }
    return a;
}

int mcm(int a , int b){
    return (a*b)/mcd(a,b);
}

int divisiores(int a){
    int res = 0;
    int i = 2;
    if(a == 1)return 1;
    while(a+1 > i){
        if(a % i == 0)res++;
        i++;
    }
    return res;
}

bool sumanIguales(int x){
    int a0,a1,a2,a3,a4,a5,restador;
    a5 = x%10;
    a4 = (x/10)%10;
    a3 = (x/100)%10;
    a2 = (x/1000)%10;
    a1 = (x/10000)%10;
    a0 = x/100000;
    //cout << a0 << endl;
    return (a0+a1+a2)==(a3+a4+a5);
}

int cantOcurrencias(int x, int y, vector<int> v, int tam){
    int contador1 = 0;
    int contador2 = 0;
    int i = 0;
    int zigzag1 = 0;
    int zigzag2 = 0;
    while(i < tam-1){
        if(v[i] == x && v[i+1] == y){
            zigzag1++;
            if(contador1 < zigzag1)contador1 = zigzag1;
            i = i+2;
        }else if(v[i] == y && v[i+1] == x){
            zigzag2++;
            if(contador2 < zigzag2)contador2 = zigzag2;
            i = i+2;
        }else{
            i++;
            zigzag1 = 0;
            zigzag2 = 0;
        }
    }
    if(contador2 < contador1){
        return contador1;
    }else{
        return contador2;
    }      
}

void generarCB(vector< pair<double, Matriz> >& avav){
  ofstream datosDeSalida ("PrimerIntento1.csv");
  for(int i = 0; i < avav.size(); i++){
    datosDeSalida << avav[i].first << ',';
    for(int j = 0; j < avav[i].second.filas(); j++){
      datosDeSalida << avav[i].second.handle(j,0);
      datosDeSalida << ',';
    }
    datosDeSalida << endl;
  }
  datosDeSalida.close();
}

int mapeo(int a, int b){
	return (pow(2,a)*((2*b)+1))-1;
}

vector<int> mapeoRet(int a){
	vector<int> v;
	if(a == 0){
		v.push_back(a);
		v.push_back(a);
		v.push_back(a);
		return v;
	} 
	int x = 0;
	int b = a;
	a = a+1;
	while(a%2 == 0){
		a = a/2;
		x++;
	}
	a = a-1;
	if(a%2 == 0) a = a/2;
	v.push_back(x);
	v.push_back(a);
	v.push_back(b);
	return v;
}

vector<int> relaciones(int x,int y){
	vector<int> v;
	int a = mapeo(x,y);
	v.push_back(a);
	a = mapeo(x+1,y);
	v.push_back(a);
	a = mapeo(x,y+1);
	v.push_back(a); 
	return v;
}

vector<int> filaX (int x, int cantC, int cantF){
	vector<int> res;
	for(int i = 0; i < cantF*cantC; i++){
		if(i == x){
			res.push_back(1);
		}
		if(x % (cantC-1) == 0 && i == x){
			res.push_back(-1);
		}else if(i == x || i == 1+x){
			res.push_back(1);
		}else{
			res.push_back(0);
		}	
	}
	return res;
}

vector<int> filaY(int x, int cantC, int cantF){
	vector<int> res;
	for(int i = 0; i < cantF*cantC; i++){
		///while i sea menor que la cantidad de pixeles 
		if(x % (cantF-1) == 0 && i == x){
			//si y esta en una de las ultimas posiciones && i es igual a aux*x 
			res.push_back(-1);
		}else if(i == x || i == x+cantC){
			//si i == y || i 
			res.push_back(1);
		}else{
			res.push_back(0);
		}	
	}
	return res;
}

vector<double> levantarDatos(vector<vector<vector<double> > > & res){
  ifstream training_set("data/train2.csv");
  if(!training_set.is_open()) throw runtime_error("No se pudo leer el archivo");
  training_set.ignore(numeric_limits<streamsize>::max(),'\n');
  //descarto los que no son numeros
  vector<double> elemXClase(10);
  while(!training_set.eof()){
    int clase_i;
    training_set >> clase_i;
    elemXClase[clase_i] += 1;
    vector<double> dato;
    //obtengo las coordenadas del vector
    while(training_set.peek() == ','){
      //ignoro la coma
      training_set.ignore();
      //leo el pixel
      double pix;
      training_set >> pix;
      //agrego el valor al vector
      dato.push_back(pix);
    }
    res[clase_i].push_back(dato);
    training_set.ignore(numeric_limits<streamsize>::max(),'\n');
    //descarto salto de linea
    training_set.peek();
    //levanta el flag de EOF si se termino el archivo
  }
  return elemXClase;
}

vector<pair<double,Matriz> > entrenar(vector<pair<double,Matriz> > res,Matriz& A,Matriz& x,int cant_vectores, int cant_iterariones){
  for(int i = 0; i < cant_vectores; i++){
    x.generarRandom();
    pair<double, Matriz> salida;
    salida = A.metodoDeLasPotencias(cant_iterariones,x);
    A = A.deflacion(salida.first, salida.second);
    cout << salida.first << endl;
    res.push_back(salida);
  }
  return res;
}

generarParticiones(archivo& datos, archivo& entrenamiento, archivo& validacion, int cant_particiones){
  int rand= 21%cant_particiones;
  for(int clases_i = 0; clases_i < datos.size();clases_i++){
    for(int elemento_i = 0; elemento_i < datos[clases_i].size();elemento_i++){
      for(int coordenada_i = 0; coordenada_i < datos[clases_i][elemento_i].size(); coordenada_i++){
        if(clases_i/cant_particiones != rand){
          entrenamiento[clases_i].push_back(datos[clases_i][elemento_i]);
        }else{
          validacion[clases_i].push_back(datos[clases_i][elemento_i]);
        }
      }
    }
  }
}

int main(){
  vector<pair<double, Matriz> > cambioBase;
  int cant_DatosValidacion;
  string entrenar;
  cout << "Desea entrenar: e (y presione Enter)"<<endl;
  cin >> entrenar;
  string validar;
  cout << "Desea validar: v (y presione Enter)"<<endl;
  cin >> validar;
  int cant_particiones;
  cout << "Ingrese la cantidad de particiones que sea hacer del dataset"<<endl;
  cin >> cant_particiones;
  if(entrenar == "e"){
    int cant_vectores;
    int max_iteraciones;
    cout <<"Ingrese cantidad de dimensiones para considerar la descomposicion: "<<endl;
    cin >> cant_vectores;
    cout <<"Ingrese cantidad de iteraciones maxima para convergencia de un autovalor: "<<endl;
    cin >> max_iteraciones;
    archivo datos(10);
    vector<double> elemXClase = levantarDatos(datos);
    int cant_Datos = 0;
    for(int i = 0; i < elemXClase.size(); i++){
      cant_Datos += elemXClase[i];
      cout << datos[i].size()<<endl;
      cout << elemXClase[i] << " "<< i <<endl;
    }
    int cant_DatosEntrenamiento;
    archivo datosEntrenamiento;
    archivo datosValidacion;
    Matriz E(cant_DatosEntrenamiento,784,false);
    generarParticiones(datos, datosEntrenamiento, datosValidacion, cant_particiones);
    cant_DatosValidacion = cant_Datos-cant_DatosEntrenamiento;
    E.cargarDatos(datosEntrenamiento);
    cout << "Datos cargados"<< endl;
    E.normalizar();
    cout << "Datos normalizados"<< endl;
    E.matrizDeCovarianza();
    cout << "Matriz de covarianza"<< endl;
    Matriz x(cant_Datos,1,false);
    cout << "Iniciando entrenamiento"<<endl;
    cambioBase = entrenar(cambioBase,E,x,cant_vectores,max_iteraciones);
    cout << "Generando salida"<< endl;
    generarCB(cambioBase);
    cout << "Fin entrenamiento"<< endl;
      }
  else{
    cout << "Opcion invalida"<< endl;
  }
  if(validar == "v"){
    Matriz V(cant_DatosValidacion,784,false);
    //cargar();
    //knn(cambioBase,datos);
    cout << "Fin validacion"<< endl;
  }else{
    cout << "Opcion invalida"<< endl;
  }
  return 0;
}
