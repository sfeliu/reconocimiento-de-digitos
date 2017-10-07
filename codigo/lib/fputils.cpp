#include "fputils.h"
#include <cmath>

const double EPS = 10e-10;

bool fputils::eq(const double a, const double b) {
    return abs(a - b) <= EPS;
}

bool fputils::lt(const double a, const double b) {
    return a - b <= EPS;
}