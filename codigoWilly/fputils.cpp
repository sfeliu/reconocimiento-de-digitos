#include "fputils.h"
#include <cmath>

const double EPS = 10e-10;

bool eq(const double a, const double b) {
    return abs(a - b) <= EPS;
}

bool lt(const double a, const double b) {
    return a - b <= EPS;
}

bool leq(const double a, const double b) {
    return !lt(b, a);
}