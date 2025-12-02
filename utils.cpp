#include "utils.h"

#include <stdio.h>
#include <math.h>

const double EPS = 1e-7;

int isEqual(double a, double b) {
    if (fabs(a - b) < EPS) {
        return 1;
    }
    
    return 0;
}
