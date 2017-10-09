#ifndef TRABAJO_MATRIZ_H
#define TRABAJO_MATRIZ_H

#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
#include <random>
#include "Matriz.cpp"

using namespace std;

class Matriz {
    int cols;
    int fils;
    double** Filas;


public:
    Matriz();
    ~Matriz();
    Matriz(const Matriz&);
    Matriz& operator=(Matriz other){
        swap(*this, other);
        return *this;
    }
    void swap(Matriz&, Matriz&);
    double ** getFilas();
    int getfils();
    int getcols();
    void print();
    double randomNum();
    void randomMat(int,int);
    void randomMatSim(int,int);
    void crearMatriz(int,int);
    void Identidad(int,int);
    void cero(int,int);
    Matriz covarianzaMat();
    void deflacion(int a);
    double esperanzaVA(int);
    double norma2();
    void operator+(Matriz&);
    void operator-(Matriz& M);
    Matriz operator*(Matriz&);
    Matriz multTranspuesta();
    double* randomVec(int);
    int pitvoteMax(int);
    void eliminacionGauss();
    Matriz FactoLU();
    double metodoPotencia();
    void transponer();
    bool esSimetrica();
    void reduccionDim(int);
    int kNN(int, Matriz&, double*, int);

};


int Matriz:: kNN(int k, Matriz& M3, double * v, int tam){//se la aplico a la matriz trainer y le paso m3 que son la clase de cada fila de la matriz trainer y v el vector de la test
    if(k<1){cerr<<"k tiene que ser mayor a 0"<<endl;}
    priority_queue<double, vector<double>, less<double> > * arryMaxHeap [10];//creo array de 9 minHeaps
    for(int j=0;j<10; j++){
        arryMaxHeap[j]= new priority_queue<double, vector<double>, less<double> >;
    }
    Matriz M1;
    M1.crearMatriz(1,tam);
    for(int h=0;h<M1.cols;h++){ //copio el vector v
        M1.Filas[0][h]=v[h];
    }
    Matriz M2F;
    M2F.crearMatriz(1,tam);
    for(int i=0;i<fils;i++){
        for(int h=0;h<M2F.cols;h++){ //copio la fila i de M2 a M2F
            M2F.Filas[0][h]=Filas[i][h];
        }
        M2F-M1;
        double norm = M2F.norma2();
        double max=0;
        int maxpos=0;
        if(i<k){
            arryMaxHeap[(int)M3.Filas[i][0]]->push(norm);
        }else{
            for(int n=0;n<10;n++){//encuentro el maximo de todos los maxheaps
                if(!arryMaxHeap[n]->empty()){
                    if(max < arryMaxHeap[n]->top()){
                        maxpos=n;
                        max=arryMaxHeap[n]->top();
                    }
                }
            }
            if(max>norm){
                arryMaxHeap[maxpos]->pop();
                arryMaxHeap[(int)M3.Filas[i][0]]->push(norm);
            }
        }
    }
    int res [10];
    for(int m=0;m<10;m++){res[m]=0;}//aca pongo en la i-esima posicion de res la cantidad de veces que el digito i aparece en el radio k.
    for(int n=0;n<10;n++){//encuentro el minimo de todos los minheaps
        while(!arryMaxHeap[n]->empty()){
            arryMaxHeap[n]->pop();
            res[n]= res[n]+1;
        }
    }
    int max=0;
    int indice=0;
    for(int r=0;r<10;r++){//busco el subindice de res tal que en esa posicion se encuentre el maximo del array.
        if(res[r]>max){
            max = res[r];
            indice =r;
        }
    }
    for(int pq=0;pq<10;pq++){
        delete arryMaxHeap[pq];
    }
    return indice;
    /*priority_queue<double, vector<double>, greater<double> > * arryMinHeap [10];//creo array de 9 minHeaps
    for(int j=0;j<10; j++){
        arryMinHeap[j]= new priority_queue<double, vector<double>, greater<double> >;
    }
    Matriz M1;
    M1.crearMatriz(1,tam);
    for(int h=0;h<M1.cols;h++){//copio el vector v
        M1.Filas[0][h]=v[h];
    }
    Matriz M2F;
    M2F.crearMatriz(1,tam);
    for(int i=0;i<fils;i++){
        for(int h=0;h<M2F.cols;h++){//copio la fila i de M2 a M2F
            M2F.Filas[0][h]=Filas[i][h];
        }
        M2F-M1;
        M2F.transponer();
        double norm = M2F.norma2();
        M2F.transponer();
        arryMinHeap[(int)M3.Filas[i][0]]->push(norm);
    }
    int res [10];
    for(int m=0;m<10;m++){res[m]=0;}
    for(int l=0;l<k;l++){//aca pongo en la i-esima posicion de res la cantidad de veces que el digito i aparece en el radio k.
        int pos=0;
        double min=100000000;
        for(int n=0;n<10;n++){//encuentro el minimo de todos los minheaps
            if(!arryMinHeap[n]->empty() and min > arryMinHeap[n]->top()){
                min=arryMinHeap[n]->top();
                pos =n;
            }
        }
        arryMinHeap[pos]->pop();
        res[pos]= res[pos]+1;
    }
    int max=0;
    int indice=0;
    for(int r=0;r<10;r++){//busco el subindice de res tal que en esa posicion se encuentre el maximo del array.
        if(res[r]>max){
            max = res[r];
            indice =r;
        }
    }
    for(int pq=0;pq<10;pq++){
        delete arryMinHeap[pq];
    }
    return indice;*/
}

Matriz::Matriz():Filas(NULL), cols(0),fils(0){}

Matriz::~Matriz(){
    for(int i=0;i<fils;i++){
        delete[] Filas[i];
    }
    delete[] Filas;
    fils =0;
    cols=0;

}

void Matriz:: swap(Matriz& M1, Matriz& M2){
    int filsTemp=M1.fils;
    int colsTemp=M1.cols;
    double** FilasTemp=M1.Filas;
    M1.fils = M2.fils;
    M1.cols = M2.cols;
    M1.Filas = M2.Filas;
    M2.fils = filsTemp;
    M2.cols = colsTemp;
    M2.Filas = FilasTemp;

}

Matriz::Matriz(const Matriz& other) {
    cols=other.cols;
    fils=other.fils;
    Filas = new double* [other.fils];
    for(int i =0 ;i<fils;i++){
        Filas[i] = new double[cols];
        for (int j = 0; j<cols ; j++) {
            Filas[i][j] = other.Filas[i][j];
        }
    }

}

void Matriz::operator+(Matriz& M) {
    if(fils != M.fils || cols != M.cols){
        cerr<< "error: No se puede sumar"<<endl;
    }
    for(int i=0;i<fils;i++){
        for(int j=0;j<cols;j++){
            Filas[i][j]= Filas[i][j] + M.Filas[i][j];
        }
    }
}

void Matriz::operator-(Matriz& M) {
    if(fils != M.fils || cols != M.cols){
        cerr<< "error: No se puede restar"<<endl;
    }
    for(int i=0;i<fils;i++){
        for(int j=0;j<cols;j++){
            Filas[i][j]= Filas[i][j] - M.Filas[i][j];
        }
    }
}

Matriz Matriz::operator*(Matriz& M) {
    if(cols != M.fils){
        cerr<<"error: no se pueden multiplicar"<<endl;
    }
    Matriz M0;
    M0.Identidad(fils,M.cols);
    for(int i=0;i<fils;i++){
        double suma = 0;
        for(int j=0;j<M.cols;j++){
            for(int k=0;k<cols;k++){
                suma = suma + Filas[i][k]*M.Filas[k][j];
            }
            M0.Filas[i][j]=suma;
            suma=0;
        }
    }
    return M0;
    //swap(*this,M0);
}

Matriz Matriz::multTranspuesta(){
    Matriz M0;
    M0.Identidad(fils,fils);
    for(int i=0;i<fils;i++){
        double suma = 0;
        for(int j=0;j<i+1;j++){
            for(int k=0;k<cols;k++){
                suma = suma + Filas[i][k]*Filas[j][k];
            }
            M0.Filas[i][j]=suma;
            suma=0;
        }
    }
    for(int i=0;i<fils;i++){
        for(int j=0;j<i;j++){
            M0.Filas[j][i]=M0.Filas[i][j];
        }
    }

    return M0;
}

double ** Matriz::getFilas() {
    return Filas;
}

int Matriz::getfils(){
    return fils;
}

int Matriz::getcols() {
    return cols;
}

void Matriz::crearMatriz(int f, int c){
    fils = f;
    cols = c;
    Filas = new double* [fils];
    for(int i=0;i<fils;i++){
        Filas[i]= new double [c];
    }

}

double Matriz::norma2(){
    double suma=0;
    if(fils ==1){
        for(int j =0;j<cols;j++){
            suma = suma + ((Filas[0][j])*(Filas[0][j]));
        }
    }else{
        for(int i =0;i<fils;i++){
            suma = suma + ((Filas[i][0])*(Filas[i][0]));
        }
    }
    suma = sqrt(suma);
    return suma;
}

void Matriz::print(){
    for(int i=0;i<fils;i++){
        int k=0;
        while(k<cols){
            if(k == cols-1 ){
                cout << Filas[i][k]<< endl;
                k++;
            }else{
                cout<< Filas[i][k] << "  ";
                k++;
            }

        }
    }
}

double Matriz::randomNum(){
    std::mt19937 rng;
    rng.seed(std::random_device()());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(0,2);
    return dist6(rng);
}

double* Matriz::randomVec(int size){
    double* fila = new double [size];
    int i=0;
    while(i < size){
        fila[i]=randomNum();
        i++;
    }
    return fila;
}

void Matriz::randomMat(int cantF, int cantC){
    cols=cantC;
    fils=cantF;
    Filas = new double* [cantF];
    int j=0;
    while(j<cantF){
        Filas[j] = randomVec(cantC);
        j++;
    }
}

void Matriz::randomMatSim(int f, int c){
    cols=c;
    fils=f;
    Filas = new double* [f];
    int j=0;
    while(j<f){
        Filas[j] = randomVec(c);
        j++;
    }
    Matriz M(*this);
    Matriz Mt(M);
    Mt.transponer();
    *this =M*Mt;

}

void Matriz::Identidad(int f ,int c){
    cols=c;
    fils=f;
    Filas = new double* [f];
    for(int i=0;i<f;i++){
        Filas[i]= new double[c];
        for(int j=0;j<c;j++){
            (i==j) ? Filas[i][j]=1 : Filas[i][j]=0;
        }
    }
}

void Matriz::cero(int f, int c){
    cols=c;
    fils=f;
    Filas = new double* [f];
    for(int i=0;i<f;i++){
        Filas[i]= new double[c];
        for(int j=0;j<c;j++){
            Filas[i][j]=0;
        }
    }
}

Matriz Matriz::covarianzaMat(){
    double esperanza =0;
    Matriz Medias;
    Medias.crearMatriz(1,784);
    for(int j=0;j<cols;j++){
        esperanza = esperanzaVA(j);
        Medias.Filas[0][j]=esperanza;
        for(int i=0;i<fils;i++){
            Filas[i][j]= Filas[i][j] - esperanza;
        }
    }
    Matriz M;
    transponer();
    M = multTranspuesta();
    for(int i =0;i<M.fils;i++){
        for(int k=0;k<M.cols;k++){
            M.Filas[i][k] = M.Filas[i][k]/(fils-1);
        }
    }
    swap (M,*this);
    return Medias;
}

double Matriz::esperanzaVA(int c){
    double suma=0;
    for(int i=0;i<fils;i++){
        suma = suma + Filas[i][c];
    }
    suma = suma/fils;
    return suma;
}

int Matriz::pitvoteMax(int c) {
    double max=0;
    int res=c;
    for(int i=c;i<fils;i++){
        if(max < Filas[i][c]){
            max = Filas[i][c];
            res=i;
        }
    }
    return res;
}

void Matriz::eliminacionGauss(){
    for(int i=0;i<fils;i++){
        int p = pitvoteMax(i);//aca busco el elemento maximo en la i-esima columna.
        if(Filas[p][i]==0){
            cerr<< "La matriz no tiene factorizacion LU"<<endl;
        }
        if(p != i){//si encontre un elemento por debajo de M[i][i], en la i-esima columan, que es mas grande entonces intercambio la fila p y la i.
            double* temp =Filas[i];
            Filas[i]=Filas[p];
            Filas[p]=temp;
        }
        for(int k=i+1;k<fils;k++){
            double m=((Filas[k][i]) / (Filas[i][i]));
            for(int j=i;j<cols;j++){
                Filas[k][j] = ((Filas[k][j]) - ((Filas[i][j])*m));
            }
        }
    }
}

Matriz Matriz::FactoLU() {
    Matriz L;
    L.cero(fils,cols);
    Matriz Permutacion;
    Permutacion.Identidad(fils,cols);
    for(int i=0;i<fils;i++){
        int p = pitvoteMax(i);//aca busco el elemento maximo en la i-esima columna.
        if(Filas[p][i]==0){
            cerr<< "La matriz no tiene factorizacion LU pura"<<endl;
        }
        if(p != i){//si encontre un elemento por debajo de M[i][i], en la i-esima columan, que es mas grande entonces intercambio la fila p y la i.
            double* temp =Filas[i];
            Filas[i]=Filas[p];
            Filas[p]=temp;
            double* Ptemp=Permutacion.Filas[i];
            Permutacion.Filas[i]=Permutacion.Filas[p];
            Permutacion.Filas[p]=Ptemp;
            double* Ltemp=L.Filas[i];           // porque hace falta transponer las filas de L.
            L.Filas[i]=L.Filas[p];
            L.Filas[p]=Ltemp;
        }
        for(int k=i+1;k<fils;k++){
            double m=((Filas[k][i]) / (Filas[i][i]));
            L.Filas[k][i]=m;
            for(int j=i;j<cols;j++){
                Filas[k][j] = ((Filas[k][j]) - ((Filas[i][j])*m));
            }
        }
    }
    for(int j=0;j<L.fils;j++){
        for(int t=0;t<L.cols;t++){
            if(j==t){L.Filas[j][t]=1;}
        }
    }
    *this=L;
    Permutacion.transponer();//porque hace falta?
    return Permutacion;
}

double Matriz::metodoPotencia() {
    Matriz V;
    V.crearMatriz(fils,1);
    for(int i=0;i<fils;i++){
        V.Filas[i][0]=1;
    }

    Matriz copy(*this);
    for(int k=0;k<10;k++){
        V=copy*V;
        double norma =V.norma2();
        for(int i=0;i<fils;i++){
            V.Filas[i][0]=(V.Filas[i][0])/norma;
        }
    }
    Matriz Vt(V);
    Vt.transponer();

    V=(*this)*V;
    double autoval=(Vt*V).Filas[0][0];
    Vt.transponer();
    swap(Vt,*this);
    return autoval;

}

void Matriz::deflacion(int a){
    Matriz V1(*this);     //V1 va a tomar el valor de los autovectores de this.
    Matriz copia(*this);//hago una copia de this porque a this lo voy a ir poniendo en la i-esima columna el i-esimo autovalor.
    Matriz autovecs;
    autovecs.crearMatriz(fils,a);
    for(int c=0;c<a;c++){
        double autoval;
        autoval = V1.metodoPotencia();  // V1 pasa a ser el autovector con con el autovalor mas grande que es autoval.

        for(int i=0; i<fils;i++){
            autovecs.Filas[i][c]=V1.Filas[i][0]; // pongo el autovector V1 en la i-esima columna de this.
        }

        Matriz M;
        M=V1.multTranspuesta(); //M es V*V^t
        for(int i=0;i<fils;i++){
            for(int j=0;j<cols;j++){
                M.Filas[i][j]= (M.Filas[i][j])*autoval;  //multiplico a cada elemento de M por el autovalor autoval.
            }
        }
        copia-M;

        V1=copia;
    }
    swap(autovecs,*this);
}

void Matriz::transponer(){
    Matriz M(*this);
    for(int i=0;i<fils;i++){
        delete[] Filas[i];
    }
    delete[] Filas;
    crearMatriz(M.cols,M.fils);
    for(int i=0;i<fils;i++){
        for(int j=0;j<cols;j++){
            Filas[i][j]=M.Filas[j][i];
        }
    }
}

bool Matriz::esSimetrica(){
    if(cols != fils){
        cerr<<"No es simetrica porque no es cuadrada"<<endl;
    }
    int i=0;
    while(i<fils){
        int j=0;
        while(j<cols){
            if(Filas[i][j] != Filas[j][i]){
                return false;
            }
            j++;
        }
        i++;
    }
    return true;
}

void Matriz::reduccionDim(int a){
    Matriz M;
    M.crearMatriz(fils,a);
    for(int i=0;i<fils;i++){
        for(int j=0;j<a;j++){
            M.Filas[i][j] = Filas[i][j];
        }
    }
    swap(*this, M);
}


#endif //TRABAJO_MATRIZ_H
