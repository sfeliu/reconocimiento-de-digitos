#ifndef __FPUTILS_H__
#define __FPUTILS_H__

#include <cmath>

using namespace std;

namespace fputils {
    
    const double EPS = 10e-10;

    inline bool eq(const double a, const double b) { return abs(a - b) <= EPS; };

    inline bool lt(const double a, const double b) { return a - b <= EPS; };
}

#endif //__FPUTILS_H__