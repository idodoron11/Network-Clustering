#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#include "spmat.h"

int main() {
    spmat *spm;
    Matrix *mat;
    int n = 10;
    srand(time(0));
    mat = createMatrix(n);
    spm = generateRandomSymSpmat(n, 10, mat);
    printSpmat(spm);
    return 0;
}
