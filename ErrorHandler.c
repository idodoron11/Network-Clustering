#include <process.h>
#include "ErrorHandler.h"

void assertMemoryAllocation(void *p){
    if(p == NULL) {
        printf(MemoryAllocationErr);
        exit(1);
    }
}

void assertFileOpen(FILE *file, char *filename){
    if(file == NULL){
        printf("%s %s. %s\n", FileOpenErr_start, filename, FileOpenErr_end);
        exit(2);
    }
}

void assertFileRead(int readAmount, int expected, char *filename){
    if(readAmount != expected){
        printf("%s %s. %s\n", FileReadErr_start, filename, FileReadErr_end);
        exit(3);
    }
}

void assertFileWrite(int writeAmount, int expected, char *filename){
    if(writeAmount != expected){
        printf("%s %s.\n", FileWriteErr, filename);
        exit(4);
    }

}

void assertBooleanStatementIsTrue(char statement){
    if(statement == 0){
        printf("%s\n", GeneralErr);
        exit(5);
    }
}

void assertBooleanStatement(char statement, char expectedBooleanValue){
    if((statement == 0 && expectedBooleanValue != 0) || (statement != 0 && expectedBooleanValue == 0)){
        printf("%s\n", GeneralErr);
        exit(5);
    }
}
