#ifndef CLUSTER_ERRORHANDLER_H
#define CLUSTER_ERRORHANDLER_H

#include <stdio.h>

static const char MemoryAllocationErr[] = "There was a problem during dynamic memory allocation.";
static const char FileOpenErr_start[] = "The program encountered a problem when opening the file:";
static const char FileOpenErr_end[] = "Make sure it exists, and is not locked by another process.";
static const char FileReadErr_start[] = "The program encountered a problem when reading from the file:";
static const char FileReadErr_end[] = "Make sure the content is valid and formatted correctly.";
static const char FileWriteErr[] = "The program encountered a problem when writing to the file:";
static const char GeneralErr[] = "The program encountered an unknown problem.";

void assertMemoryAllocation(void *p);

void assertFileOpen(FILE *file, char *filename);

void assertFileRead(int readAmount, int expected, char *filename);

void assertFileWrite(int writeAmount, int expected, char *filename);

void assertBooleanStatement(char statement);

#endif
