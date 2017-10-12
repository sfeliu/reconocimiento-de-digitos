#include <ctime>
#include <stdio.h>

using namespace std;

class Timer {    
    public:
        Timer() : _start(clock()) {};
        void iniciar() { _start = clock(); };
        double tiempo() const { return (double) (clock() - _start) / CLOCKS_PER_SEC; };
        
    private:
        clock_t _start;
};