/**
 * tester for cluster project - Neo
 * the graphs that are tester for are:
 *
 * a means almost clique and c means clique
 *
 * 3empty
 * 30empty
 *
 * 3c
 * 30c
 * 30a
 * 300c
 * 300a
 *
 * 20-30c
 * 20-30a
 * 60-100c
 * 60-100a
 */

#include <stdlib.h>
#include <stdio.h>
#include "../../cluster.h"
#include "../testUtils.h"

/**
void printA(int* a, int size){
    int i;
    printf("\n");
    for(i = 0; i < size ; i++){
        printf("%d ",a[i]);
    }
}**/

int equalArray(int *a1, int *a2, int size) {
    int i;
    for (i = 0; i < size; i++) {
        if (a1[i] != a2[i]) {
            return 0;
        }
    }
    return 1;
}

/** returns 1 iff correct **/
int check(char *fileName, int *array1, int *array2, int numberOfGroupsExpected, int size) {
    int numOfGroups;
    int rank;
    int *a1;
    int *a2;
    int i;
    int temp;
    int count = 0;
    int rank1;
    int rank2;
    FILE *resFile = fopen(fileName, "r");

    fread(&numOfGroups, sizeof(int), 1, resFile);

    if (numOfGroups != numberOfGroupsExpected) {
        return 0;
    }

    if (numberOfGroupsExpected == 1) {
        fread(&rank, sizeof(int), 1, resFile);

        if (rank != size) {
            return 0;
        }

        a1 = (int *) malloc(sizeof(int) * size);
        while (fread(&temp, sizeof(int), 1, resFile)) {
            if (temp >= size || temp < 0) {
                return 0;
            }
            a1[temp] = 0;
            count++;
        }

        if (count != size) {
            return 0;
        }

        if (equalArray(a1, array1, size) == 0) {
            return 0;
        } else {
            return 1;
        }
    } else {
        /** number of expected = 2 **/
        fread(&rank1, sizeof(int), 1, resFile);
        a1 = (int *) calloc(size, sizeof(int));

        for (i = 0; i < rank1; i++) {
            fread(&temp, sizeof(int), 1, resFile);
            if (temp >= size || temp < 0) {
                return 0;
            }
            a1[temp] = 1;
        }

        fread(&rank2, sizeof(int), 1, resFile);
        a2 = (int *) calloc(size, sizeof(int));

        for (i = 0; i < rank2; i++) {
            fread(&temp, sizeof(int), 1, resFile);
            if (temp >= size || temp < 0) {
                return 0;
            }
            a2[temp] = 1;
        }

        if (fread(&temp, sizeof(int), 1, resFile) != 0) {
            return 0;
        }

        if (equalArray(a1, array1, size)) {
            if (equalArray(a2, array2, size)) {
                return 1;
            } else {
                return 0;
            }
        } else {
            if (equalArray(a1, array2, size)) {
                if (equalArray(a2, array1, size)) {
                    return 1;
                } else {
                    return 0;
                }
            } else {
                return 0;
            }
        }
    }
}


int main() {
    int i;

    /** ---------------------- making res arrays that we know are results ---------------------- **/
    int threeEmpty[] = {0, 0, 0};
    int threeC[] = {0, 0, 0};
    int thirtyC[30];
    int thirtyA[30];
    int thirtyEmpty[30];
    int threeHundredC[300];
    int threeHundredA[300];
    int twentyThirty1[50];
    int twentyThirty2[50];
    int sixtyHundred1[160];
    int sixtyHundred2[160];
    char *argv[3];
    int argc = 3;
    int flag;
    int allGood = 1;

    for (i = 0; i < 30; i++) {
        thirtyEmpty[i] = 0;
    }


    for (i = 0; i < 30; i++) {
        thirtyC[i] = 0;
    }

    for (i = 0; i < 30; i++) {
        thirtyA[i] = 0;
    }

    for (i = 0; i < 300; i++) {
        threeHundredC[i] = 0;
    }

    for (i = 0; i < 300; i++) {
        threeHundredA[i] = 0;
    }

    for (i = 0; i < 50; i++) {
        if (i < 20) {
            twentyThirty1[i] = 1;
        } else {
            twentyThirty1[i] = 0;
        }
    }

    for (i = 0; i < 50; i++) {
        if (i >= 20) {
            twentyThirty2[i] = 1;
        } else {
            twentyThirty2[i] = 0;
        }
    }

    for (i = 0; i < 160; i++) {
        if (i < 60) {
            sixtyHundred1[i] = 1;
        } else {
            sixtyHundred1[i] = 0;
        }
    }

    for (i = 0; i < 160; i++) {
        if (i >= 60) {
            sixtyHundred2[i] = 1;
        } else {
            sixtyHundred2[i] = 0;
        }
    }

    /** ------------------------- making outFiles ------------------------- **/

    argv[0] = "name";

    argv[1] = "3empty";
    argv[2] = "3emptyOut";
    compareExpected(argv[1], cluster(argc, argv), threeEmpty);

    argv[1] = "30empty";
    argv[2] = "30emptyOut";
    compareExpected(argv[1], cluster(argc, argv), thirtyEmpty);

    argv[1] = "3c";
    argv[2] = "3cOut";
    compareExpected(argv[1], cluster(argc, argv), threeC);

    argv[1] = "30c";
    argv[2] = "30cOut";
    compareExpected(argv[1], cluster(argc, argv), thirtyC);

    argv[1] = "30a";
    argv[2] = "30aOut";
    compareExpected(argv[1], cluster(argc, argv), thirtyA);

    argv[1] = "300c";
    argv[2] = "300cOut";
    compareExpected(argv[1], cluster(argc, argv), threeHundredC);

    argv[1] = "300a";
    argv[2] = "300aOut";
    compareExpected(argv[1], cluster(argc, argv), threeHundredA);

    argv[1] = "20-30c";
    argv[2] = "20-30cOut";
    compareExpected(argv[1], cluster(argc, argv), twentyThirty1);

    argv[1] = "20-30a";
    argv[2] = "20-30aOut";
    compareExpected(argv[1], cluster(argc, argv), twentyThirty2);

    argv[1] = "60-100c";
    argv[2] = "60-100cOut";
    compareExpected(argv[1], cluster(argc, argv), sixtyHundred1);

    argv[1] = "60-100a";
    argv[2] = "60-100aOut";
    compareExpected(argv[1], cluster(argc, argv), sixtyHundred2);


    /** ---------- checking if outFiles from our code is the same as expected result arrays ---------- **/
    flag = check("3emptyOut", threeEmpty, NULL, 1, 3);
    if (flag == 0) {
        printf("Error in 3empty graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("3empty graph is correct\n");
    }

    flag = check("30emptyOut", thirtyEmpty, NULL, 1, 30);
    if (flag == 0) {
        printf("Error in 30empty graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("30empty graph is correct\n");
    }

    flag = check("3cOut", threeC, NULL, 1, 3);
    if (flag == 0) {
        printf("Error in 3 clique graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("3 clique graph is correct\n");
    }

    flag = check("30cOut", thirtyC, NULL, 1, 30);
    if (flag == 0) {
        printf("Error in 30 clique graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("30 clique graph is correct\n");
    }

    flag = check("30aOut", thirtyA, NULL, 1, 30);
    if (flag == 0) {
        printf("Error in 30 almost clique graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("30 almost clique graph is correct\n");
    }

    flag = check("300cOut", threeHundredC, NULL, 1, 300);
    if (flag == 0) {
        printf("Error in 300 clique graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("300 clique graph is correct\n");
    }

    flag = check("300aOut", threeHundredA, NULL, 1, 300);
    if (flag == 0) {
        printf("Error in 300 almost clique graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("300 almost clique graph is correct\n");
    }

    flag = check("20-30cOut", twentyThirty1, twentyThirty2, 2, 50);
    if (flag == 0) {
        printf("Error in 20-30 cliques graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("20-30 cliques graph is correct\n");
    }

    flag = check("20-30aOut", twentyThirty1, twentyThirty2, 2, 50);
    if (flag == 0) {
        printf("Error in 20-30 cliques graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("20-30 cliques graph is correct\n");
    }

    flag = check("60-100cOut", sixtyHundred1, sixtyHundred2, 2, 160);
    if (flag == 0) {
        printf("Error in 60-100 cliques graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("60-100 cliques graph is correct\n");
    }

    flag = check("60-100aOut", sixtyHundred1, sixtyHundred2, 2, 160);
    if (flag == 0) {
        printf("Error in 60-100 almost cliques graph\n");
        allGood = 0;
    } else if (flag == 1) {
        printf("60-100 almost cliques graph is correct\n");
    }

    if (allGood == 1) {
        printf("passed all tests\n");
    } else {
        printf("did not pass all tests\n");
    }

    /** ------------------- removing outFiles ------------------- **/
    remove("3emptyOut");
    remove("30emptyOut");
    remove("3cOut");
    remove("30cOut");
    remove("30aOut");
    remove("300cOut");
    remove("300aOut");
    remove("20-30cOut");
    remove("20-30aOut");
    remove("60-100cOut");
    remove("60-100aOut");
    return 0;
}