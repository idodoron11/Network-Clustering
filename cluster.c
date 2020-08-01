#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"
#include "spmat.h"

int main() {
    spmat *spm;
    srand(time(0));
    spm = generateRandomSpmat(10, 10);
    printSpmat(spm);
    return 0;
}
