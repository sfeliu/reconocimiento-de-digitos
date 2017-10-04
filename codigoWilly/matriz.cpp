#include "matriz.h"

using namespace std;

void Matriz::_crearMatriz(const int f, const int c, const bool init) {
    if (f <= 0 || c <= 0) {
        throw domain_error("Error: la cantidad de filas y columnas debe ser mayor a cero.");
    }
    _filas = f;
    _columnas = c;
    _traspuesta = false;
    _matriz = new double*[_filas];
    for (int i = 0; i < _filas; ++i) {
        if (init)
            _matriz[i] = new double[_columnas]();
        else
            _matriz[i] = new double[_columnas];
    }
    _cols = new int[_columnas];
    for (int i = 0; i < _columnas; ++i) {
        _cols[i] = i;
    }
}

Matriz::Matriz() : _filas(0), _columnas(0), _matriz(NULL), _traspuesta(false), _cols(NULL) {}

Matriz::Matriz(const Matriz &o) {
    _crearMatriz(o.filas(), o.columnas(), false);
    for (int i = 0; i < filas(); ++i) {
        for (int j = 0; j < columnas(); ++j) {
            (*this)(i,j) = o(i,j);
        }
    }
}

Matriz::Matriz(const int n) {
    _crearMatriz(n, n);
}

Matriz::Matriz(const int f, const int c, const bool init) {
    _crearMatriz(f, c, init);
}

Matriz::Matriz(const int f, const int c, const double a[], const int n) {
    _crearMatriz(f, c);
    int i = 0, j = 0, k = 0;
    while (k < n) {
        (*this)(i,j) = a[k];
        j = (j+1) % c;
        i = j == 0 ? i+1 : i;
        ++k;
    }
}

Matriz::~Matriz() {
    for (int i = 0; i < _filas; ++i) {
        delete[] _matriz[i];
    }
    delete[] _matriz;
}

Matriz Matriz::Identidad(const int n) {
    Matriz M(n);
    for (int i = 0; i < n; ++i) {
        M(i,i) = 1;
    }
    return M;
}

void Matriz::swap(Matriz &o) {
    std::swap(_filas, o._filas);
    std::swap(_columnas, o._columnas);
    std::swap(_traspuesta, o._traspuesta);
    std::swap(_matriz, o._matriz);
    std::swap(_cols, o._cols);
}

Matriz& Matriz::operator=(Matriz o) {
    swap(o);
    return *this;
}

double& Matriz::operator()(const int i, const int j) {
    _verificarRango(i,j);
    if (_traspuesta)
        return _matriz[j][_cols[i]];
    else
        return _matriz[i][_cols[j]];
}

const double& Matriz::operator()(const int i, const int j) const {
    _verificarRango(i,j);
    if (_traspuesta)
        return _matriz[j][_cols[i]];
    else
        return _matriz[i][_cols[j]];
}

bool Matriz::operator==(const Matriz &o) const {
    if (filas() != o.filas() || columnas() != o.columnas())
        return false;
    for (int i = 0; i < filas(); ++i) {
        for (int j = 0; j < columnas(); ++j) {
            if (!eq((*this)(i,j), o(i,j)))
                return false;
        }
    }
    return true;
}

bool Matriz::operator!=(const Matriz &o) const {
    return !(*this == o);
}


Matriz& Matriz::operator*(Matriz &o){
    if (columnas() != o.filas()) {
        throw domain_error("Error: la multiplicación no esta definida para matrices de estas dimensiones");
    }
    Matriz *res = new Matriz(filas(), o.columnas(), false);
    for (int i = 0; i < res->filas(); ++i) {
        for (int j = 0; j < res->columnas(); ++j) {
            (*res)(i,j) = 0;
            for (int k = 0; k < columnas(); ++k) {
                (*res)(i,j) += (*this)(i,k) * o(k,j);
            }
        }
    }
    return *res;
}

Matriz& Matriz::operator*(double c){
    Matriz *res = new Matriz(filas(), columnas(), false);
    for (int i = 0; i < res->filas(); ++i) {
        for (int j = 0; j < res->columnas(); ++j) {
            (*res)(i,j) = (*this)(i,j) * c;
        }
    }
    return *res;
}
Matriz& Matriz::operator-(Matriz &o){
  if (columnas() != o.columnas() || filas() != o.filas()) {
    throw domain_error("Error: la suma no esta definida para matrices de estas dimensiones");
  }
  Matriz *res = new Matriz(filas(), columnas(), false);
  for (int i = 0; i < res->filas(); ++i) {
    for (int j = 0; j < res->columnas(); ++j) {
      (*res)(i,j) = (*this)(i,j) - o(i,j);
    }
  }
  return *res;
}


Matriz& Matriz::operator+(const Matriz &o) const {
    if (columnas() != o.columnas() || filas() != o.filas()) {
        throw domain_error("Error: la suma no esta definida para matrices de estas dimensiones");
    }
    Matriz *res = new Matriz(filas(), columnas(), false);
    for (int i = 0; i < res->filas(); ++i) {
        for (int j = 0; j < res->columnas(); ++j) {
            (*res)(i,j) = (*this)(i,j) + o(i,j);
        }
    }
    return *res;
}

int Matriz::filas() const {
    return _traspuesta ? _columnas : _filas;
}

int Matriz::columnas() const {
    return _traspuesta ? _filas : _columnas;
}

bool Matriz::esCuadrada() const {
    return _filas == _columnas;
}

Matriz& Matriz::trasponer() {
    _traspuesta = !_traspuesta;
    return *this;
}

Matriz Matriz::traspuesta() const {
    Matriz A(*this);
    return A.trasponer();
}

Matriz& Matriz::invertir() {
    if (!esCuadrada())
        throw domain_error("La matriz no es cuadrada.");
    Matriz A(*this);
    Matriz I = Matriz::Identidad(filas());
    A.eliminacionGaussiana(I);
    if (eq(A(A.filas()-1, A.filas()-1), 0))
        throw domain_error("La matriz no tiene inversa");
    A.backwardSubstitution(*this, I);
    return *this;
}

Matriz& Matriz::inversa() const {
    Matriz A(*this);
    return A.invertir();
}

double Matriz::normaF() const {
    double suma = 0;
    for (int i = 0; i < filas(); ++i) {
        for (int j = 0; j < columnas(); ++j) {
            suma += pow((*this)(i,j), 2);
        }
    }
    return pow(suma, 0.5);
}

Matriz& Matriz::multiplicarPorTraspuesta() {
    Matriz A(filas());
    for (int i = 0; i < A.filas(); ++i) {
        for (int j = i; j < A.filas(); ++j) {
            for (int k = 0; k < columnas(); ++k)
                A(i,j) += (*this)(i,k) * (*this)(j,k);
            A(j,i) = A(i,j);
        }
    }
    swap(A);
    return *this;
}

Matriz& Matriz::multiplicarPorTraspuestaBanda(const int p, const int q) {
    _verificarBanda(p, q);
    Matriz A(filas());
    int h = filas() <= columnas() ? filas() : min(filas(), columnas() + q);
    for (int i = 0; i < h; ++i) {
        for (int j = i; j < min(h, i+q+1); ++j) {
            for (int k = max(0, max(i,j)-q); k < min(columnas(), min(i,j)+p+1); ++k)
                A(i,j) += (*this)(i,k) * (*this)(j,k);
            A(j,i) = A(i,j);
        }
    }
    swap(A);
    return *this;
}

void Matriz::eliminacionGaussiana() {
    _eliminacionGaussiana(NULL);
}

void Matriz::eliminacionGaussiana(Matriz &b) {
    _eliminacionGaussiana(&b);
}

void Matriz::eliminacionGaussianaBanda(const int p, const int q) {
    _eliminacionGaussianaBanda(p, q, NULL);
}

void Matriz::eliminacionGaussianaBanda(const int p, const int q, Matriz &b) {
    _eliminacionGaussianaBanda(p,q, &b);
}

void Matriz::factorizacionPLU(Matriz &P, Matriz &L, Matriz &U) {
    if (!esCuadrada())
        throw domain_error("La matriz debe ser cuadrada.");
    P = L = Identidad(filas());
    U = *this;
    int f = 0, c = 0; // fila y columna del pivote
    while (f < U.filas() && c < U.columnas()) {
        int m = f; // filate del pivote de maximo valor absoluto
        for (int i = m + 1; i < U.filas(); ++i) {
            if (fabs(U(m,c)) < fabs(U(i,c)))
                m = i;
        }
        if (!eq(U(m,c), 0)) {
            U.permutarFila(f, m);
            P.permutarFila(f, m);
            for (int i = f + 1; i < U.filas(); ++i) {
                if (!eq(U(i,c), 0)) {
                    L(i,c) = U(i,c) / U(f,c);
                    U.restarMultiploDeFila(i, f, L(i,c));
                }
            }
            ++f;
        }
        ++c;
    }
}

Matriz& Matriz::backwardSubstitution(Matriz &x, const Matriz &b) {
    if (b.filas() != filas())
        throw domain_error("La cantidad de filas de la matriz parametro debe coincidicar con la de la matriz.");
    x = Matriz(columnas(), b.columnas(), false);
    for (int i = x.filas()-1; i >= 0; --i) {
        for (int k = 0; k < x.columnas(); ++k) {
            x(i,k) = b(i,k);
            for (int j = i + 1; j < x.filas(); ++j) {
                x(i,k) -= (*this)(i,j) * x(j,k);
            }
            x(i,k) /= (*this)(i,i);
        }
    }
    return x;
}

Matriz& Matriz::forwardSubstitution(Matriz &x, const Matriz &b) {
    if (b.filas() != filas())
        throw domain_error("La cantidad de filas de la matriz parametro debe coincidicar con la de la matriz.");
    x = Matriz(columnas(), b.columnas(), false);
    for (int i = 0; i < x.filas(); ++i) {
        for (int k = 0; k < x.columnas(); ++k) {
            x(i,k) = b(i,k);
            for (int j = 0; j < i; ++j) {
                x(i,k) -= (*this)(i,j) * x(j,k);
            }
            x(i,k) /= (*this)(i,i);
        }
    }
    return x;
}

Matriz& Matriz::factorizacionCholesky(Matriz &L) const {
    if (!esCuadrada())
        throw domain_error("La matriz debe ser cuadrada.");
    int n = filas();
    L = Matriz(n);
    for (int j = 0; j < n; ++j) {
        L(j,j) = (*this)(j,j);
        for (int k = 0; k < j; ++k)
            L(j,j) -= pow(L(j,k), 2);
        if (leq(L(j,j), 0))
            throw domain_error("La matriz no tiene factorizacion Cholesky.");
        L(j,j) = pow(L(j,j), 0.5);
        for (int i = j+1; i < n; ++i) {
            L(i,j) = (*this)(i,j);
            for (int k = 0; k < j; ++k)
                L(i,j) -= L(i,k)*L(j,k);
            L(i,j) /= L(j,j);
        }
    }
    return L;
}

Matriz& Matriz::factorizacionCholeskyBanda(const int p, Matriz &L) const {
    if (!esCuadrada())
        throw domain_error("La matriz debe ser cuadrada.");
    int n = filas();
    L = Matriz(n);
    for (int j = 0; j < n; ++j) {
        L(j,j) = (*this)(j,j);
        for (int k = max(0, j-p); k < j; ++k)
            L(j,j) -= pow(L(j,k), 2);
        if (leq(L(j,j), 0))
            throw domain_error("La matriz no tiene factorizacion Cholesky.");
        L(j,j) = pow(L(j,j), 0.5);
        for (int i = j+1; i < min(n, j+p+1); ++i) {
            L(i,j) = (*this)(i,j);
            for (int k = max(0, i-p); k < j; ++k)
                L(i,j) -= L(i,k)*L(j,k);
            L(i,j) /= L(j,j);
        }
    }
    return L;
}

void Matriz::permutarFila(const int i, const int j) {
    _verificarRango(i, 0);
    _verificarRango(j, 0);
    if (_traspuesta)
        std::swap(_cols[i], _cols[j]);
    else
        std::swap(_matriz[i], _matriz[j]);
}

void Matriz::multiplicarFilaPorEscalar(const int i, const double c) {
    for (int j = 0; j < columnas(); ++j)
        (*this)(i,j) *= c;
}

void Matriz::restarMultiploDeFila(const int i, const int j, const double c) {
    _verificarRango(i, 0);
    _verificarRango(j, 0);
    for (int k = 0; k < columnas(); ++k)
        (*this)(i, k) -= c * (*this)(j,k);
}

void Matriz::restarMultiploDeFilaBanda(const int p, const int q, const int i, const int j, const double c) {
    _verificarRango(i, 0);
    _verificarRango(j, 0);
    for (int k = max(0, max(i,j)-q); k < min(columnas(), min(i,j)+p); ++k)
        (*this)(i, k) -= c * (*this)(j,k);
}

void Matriz::_verificarRango(const int f, const int c) const {
    if (!(0 <= f && f < filas() && 0 <= c && c < columnas()))
        throw domain_error("Los indices de fila y/o columna estan fuera de rango.");
}

void Matriz::_verificarBanda(const int p, const int q) const {
    if (p+1 > columnas() || q+1 > filas())
        throw domain_error("La banda no puede superar la cantidad filas y columnas.");
}

void Matriz::_eliminacionGaussiana(Matriz *b) {
    int f = 0, c = 0;
    while (f < filas() && c < columnas()) {
        // Busco fila de pivoteo parcial
        int i = f; // fila de pivoteo parcial
        for (int k = i + 1; k < filas(); ++k) {
            if (fabs((*this)(i,c)) < fabs((*this)(k,c)))
                i = k;
        }
        if (!eq((*this)(i,c), 0)) { // si hay fila de pivoteo parcial
            // Hago el pivoteo (se rompe el invariante de banda p, q)
            if (b) b->permutarFila(f, i);
            permutarFila(f, i);
            for (int k = f + 1; k < filas(); ++k) {
                if (!eq((*this)(k,c), 0)) {
                    double m = (*this)(k,c) / (*this)(f,c);
                    if (b) b->restarMultiploDeFila(k, f, m);
                    restarMultiploDeFila(k, f, m);
                }
            }
            ++f;
        }
        ++c;
    }
}

void Matriz::_eliminacionGaussianaBanda(const int p, const int q, Matriz *b) {
    _verificarBanda(p, q);
    int f = 0, c = 0;
    while (f < filas() && c < columnas()) {
        // Busco fila de pivoteo parcial
        int i = f; // fila de pivoteo parcial
        for (int k = i + 1; k < min(filas(), i+q); ++k) {
            if (fabs((*this)(i,c)) < fabs((*this)(k,c)))
                i = k;
        }
        if (!eq((*this)(i,c), 0)) { // si hay fila de pivoteo parcial
            // Hago el pivoteo (se rompe el invariante de banda p, q)
            if (b) b->permutarFila(f, i);
            permutarFila(f, i);
            // Elimino la columna del pivote
            for (int k = f + 1; k < min(filas(), i+q); ++k) {
                if (!eq((*this)(k,c), 0)) { // elimino la fila solo si hace falta
                    double m = (*this)(k,c) / (*this)(f,c);
                    if (b) b->restarMultiploDeFila(k, f, m);
                    // Deshago el pivoteo parcial temporalmente para restablecer el invariante de banda p, q
                    permutarFila(f,i);
                    // La fila del pivote es ahora la fila i. Luego tengo que hacer fila(k) - m * fila(i)
                    // Pero si k == i, la fila a eliminar es ahora f. Luego tengo que hacer fila(f) - m * fila(i)
                    restarMultiploDeFilaBanda(p, q, k == i ? f : k, i, m);
                    // Hago de vuelta el pivoteo parcial
                    permutarFila(f,i);
                }
            }
            ++f;
        }
        ++c;
    }
}

Matriz& operator*(double c, Matriz &m) {
    return m * c;
}

vector<double> Matriz::medias(){
  vector<double> res;
  for(int i = 0; i < _columnas; ++i){
    double media_i = 0;
    for(int j = 0; j < _filas; ++j){
      media_i += (*this)(j,i);
    }
    media_i = media_i/_filas;
    res.push_back(media_i);
  }
  return res;
}


Matriz& Matriz::normalizar(){
  vector<double> vMedias = medias();
  for(int i = 0; i < _columnas; ++i){
    for(int j = 0; j < _filas; ++j){
      (*this)(j,i) = (*this)(j,i) - vMedias[i];
    }
  }
  return *this;
}


Matriz& Matriz::matrizDeCovarianza(){
  (*this)/(sqrt(_filas));
  return (*this).multiplicarPorTraspuesta();
}

Matriz& Matriz::generarRandom(){
  for(int i = 0; i < _filas; i++){
    for(int j = 0; j < _columnas; j++){
      _matriz[i][j] = rand()%31;
    }
  }
  return (*this)/norma2();
}

Matriz& Matriz::trasponerAux(){
  for(int i = 0; i < _filas; i++){
    for(int j = 0; j < _columnas; j++){
      _matriz[i][j] = _matriz[j][i];
    }
  }
  int aux = _filas;
  _filas = _columnas;
  _columnas = aux;
  return *this;
}

pair<double,Matriz> Matriz::metodoDeLasPotencias(int cantIter, Matriz& x){
  x = x/x.norma2();
  for(int i = 0; i < cantIter; i++){
    x = (*this)*x;
    x = x/x.norma2();
  }
  double lamba = autoValor(x,*this);
  return make_pair(lamba,x);
}

Matriz& Matriz::deflacion(double lambda,Matriz& B){
  B.multiplicarPorTraspuesta();
  B = B*lambda;
  (*this) = (*this)-B;
  return *this;
}

Matriz& Matriz::MatrizXVector(Matriz &o){
  Matriz *res = new Matriz(filas(),o.columnas(), false);
  for(int k = 0; k < filas(); k++){
    (*res)(k,0) = 0;
  }
  for (int i = 0; i < filas(); ++i) {
    for (int j = 0; j < columnas(); ++j) {
      (*res)(j,0) += (*this)(i,j)*(o(j,0));
    }
  }
  return *res;
}

double Matriz::autoValor(Matriz& x, Matriz& B){
  Matriz y = B*x;
  double res = x.norma2();
  y = x.trasponer()*y;
  x.trasponer();
  y = y/res;
  return y(0,0);
}

double Matriz::norma2(){
  int i = 0;
  double res = 0;
  while(i < _filas){
    int j = 0;
    while(j < _columnas){
      res += pow(_matriz[i][j],2);
      j++;
    }
    i++;
  }
  return sqrt(res);
}

Matriz& Matriz::operator/(double div){
  for (int i = 0; i < _filas; ++i) {
    for (int j = 0; j < _columnas; ++j) {
      (*this)(i,j) = (*this)(i,j) / div;
    }
  }
  return *this;
}


double Matriz::norma(Matriz& x){
  double res = 0;
  for (int i = 0; i < _filas; ++i) {
    for (int j = 0; j < _columnas; ++j) {
      res += (*this)(i,j);
    }
  }
  return res/_filas;
}

Matriz& Matriz::cargarDatos(vector< vector< vector <double> > > datos){
  int clases_i = 0;
  while(clases_i < datos.size()){
    int elemento_i = 0;
    while(elemento_i < datos[clases_i].size()){
      int coordenada_i = 0;
      while(coordenada_i < datos[clases_i][elemento_i].size()){
        (*this)(elemento_i,coordenada_i) = datos[clases_i][elemento_i][coordenada_i];
        coordenada_i++;
      }
      elemento_i++;
    }
    clases_i++;
  }
  return *this;
}


double Matriz::handle(int i, int j)const{
  return _matriz[i][j];
}
