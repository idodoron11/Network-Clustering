#ifndef _SPMAT_H
#define _SPMAT_H

typedef struct _spmat {
    /* Matrix capacity (n*n) */
    int n;

    /* Adds row i the matrix. Called before any other call,
     * exactly n times in order (i = 0 to n-1) */
    void (*add_row)(struct _spmat *A, const double *row, int i);

    /* Frees all resources used by A */
    void (*free)(struct _spmat *A);

    /* Multiplies matrix A by vector v, into result (result is pre-allocated) */
    void (*mult)(const struct _spmat *A, const double *v, double *result);

    /* Private field for inner implementation.
     * Should not be read or modified externally */
    void *private;
} spmat;

/* linked list implementation starts here */
struct linked_list {
    double value;
    int colind;
    struct linked_list *next;
} linked_list;
typedef struct linked_list node;
typedef node *nodeRef;

/* Allocates a new linked-lists sparse matrix of capacity n */
spmat *spmat_allocate_list(int n);


#endif
