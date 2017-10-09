#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include "Matriz.h"


using namespace std;

int main() {

    int alpha=50;
    int vecindad=15;

    ifstream infile;
    ifstream infile2;
    ofstream outfile;
    outfile.open("/Users/pablo2martin/ClionProjects/trabajo/resultados.txt");
    infile.open("/Users/pablo2martin/ClionProjects/trabajo/test.txt");
    infile2.open("/Users/pablo2martin/ClionProjects/trabajo/train.txt");
    if(infile.is_open() and infile2.is_open()){
        string str;
        double myNum=0;
        Matriz test;
        test.crearMatriz(28000,784);
        infile.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        int k=0;
        while(k<28000){
            getline(infile,str);
            stringstream ss(str);
            int j=0;
            while(j<784){
                getline(ss,str,',');
                stringstream ss2(str);
                ss2>>myNum;
                test.getFilas()[k][j]=myNum;
                j++;
            }
            k++;
        }

        string line;
        double myNum2=0;
        Matriz Train;
        Train.crearMatriz(42000,784);
        Matriz M3;
        M3.crearMatriz(42000,1);
        infile2.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        int t=0;
        while(t<42000){
            getline(infile2,line);
            stringstream stream(line);
            int i=0;
            while(i<785){
                getline(stream, line, ',');
                stringstream stream1 (line);
                stream1>>myNum2;
                if (i==0){
                    M3.getFilas()[t][0]=myNum2;
                }else{
                    Train.getFilas()[t][i-1] = myNum2;
                }
                i++;
            }
            t++;
        }

        // creo la matriz de covarianza y transformo las resduco la dimension de los datos.
        Matriz TrainAutovec(Train);

        cout<<"calculando matriz de covarianza"<<endl;
        std::clock_t start;
        double duration;
        start = std::clock();
        Matriz Medias;
        Medias = TrainAutovec.covarianzaMat();
        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cout<<"Tiempo de calculo de matrices de covarianza: "<< duration <<'\n';

        cout<<"aplicando metodo de deflacion a la matriz de covarianza"<<endl;
        start = std::clock();
        TrainAutovec.deflacion(alpha);
        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cout<<"Tiempo de calcula de los Autovectores de las matrices de covarianza: "<< duration <<'\n';

        for(int i=0;i<test.getfils();i++){
            for(int j=0;j<test.getcols();j++){
                test.getFilas()[i][j]=((test.getFilas()[i][j]-Medias.getFilas()[0][j])/sqrt(42000-1));
            }
        }
        for(int i=0;i<Train.getfils();i++){
            for(int j=0;j<Train.getcols();j++){
                Train.getFilas()[i][j]=((Train.getFilas()[i][j]-Medias.getFilas()[0][j])/sqrt(42000-1));
            }
        }

        TrainAutovec.transponer();
        Train.transponer();
        test.transponer();

        start = std::clock();
        Train = TrainAutovec*Train;
        test = TrainAutovec*test;
        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cout<<"applicar la transformacion de matriz tc(Xi)=(V^t)*Xi tardo:"<< duration <<'\n';

        Train.transponer();
        test.transponer();

        //aplico kNN
        start = std::clock();
        outfile<<"ImageId,Label"<<endl;
        for(int a=0;a<k;a++){
            int res;
            res = Train.kNN(vecindad,M3,test.getFilas()[a],alpha);
            outfile<<a+1<<","<<res<<endl;
        }
        duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cout<<"Tiempo de calculo de kNN: "<< duration <<'\n';
        infile.close();
        infile2.close();
        outfile.close();

    }else{
        cerr<<"error al abrir file"<<endl;
    }

    return 0;
}