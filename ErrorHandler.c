#include <stdlib.h>
#include "ErrorHandler.h"

/**
 * Throw an error
 * @param msg error message
 */
void throw(char *msg) {
    printf("%s", msg);
    exit(10);
}

/**
 * Assert that memory was allocated
 * @param p allocation result
 */
void assertMemoryAllocation(void *p) {
    if (p == NULL) {
        printf(MemoryAllocationErr);
        exit(1);
    }
}

/**
 * Assert that file was opened
 * @param file
 * @param filename
 */
void assertFileOpen(FILE *file, char *filename) {
    if (file == NULL) {
        printf("%s %s. %s\n", FileOpenErr_start, filename, FileOpenErr_end);
        exit(2);
    }
}

/**
 * Assert that file was read
 * @param readAmount the amount that was actually read
 * @param expected the amount to read
 * @param filename
 */
void assertFileRead(int readAmount, int expected, char *filename) {
    if (readAmount != expected) {
        printf("%s %s. %s\n", FileReadErr_start, filename, FileReadErr_end);
        exit(3);
    }
}

/**
 * Assert that file was written
 * @param writeAmount the amount that was actually written
 * @param expected the amount to write
 * @param filename
 */
void assertFileWrite(int writeAmount, int expected, char *filename) {
    if (writeAmount != expected) {
        printf("%s %s.\n", FileWriteErr, filename);
        exit(4);
    }

}

/**
 * Assert value of boolean variable is true
 * @param statement
 */
void assertBooleanStatementIsTrue(char statement) {
    if (statement == 0) {
        printf("%s\n", GeneralErr);
        exit(5);
    }
}

/**
 * Assert value of boolean variable
 * @param statement
 * @param expectedBooleanValue expected value
 */
void assertBooleanStatement(char statement, char expectedBooleanValue) {
    if ((statement == 0 && expectedBooleanValue != 0) || (statement != 0 && expectedBooleanValue == 0)) {
        printf("%s\n", GeneralErr);
        exit(5);
    }
}
