#include <stdio.h>
#include <stdlib.h>
#include "spmat.h"
#include "matrix.h"
#include "ErrorHandler.h"

/* linked list implementation starts here */
struct linked_list {
    double value;
    int colind;
    struct linked_list *next;
} linked_list;
typedef struct linked_list node;
typedef node *nodeRef;

/* linked list operations */
/**
 * Creates a linked list of non-zero items from a given row.
 * Each list-node consists of value, column index, and a pointer to the
 * next item in the list (NULL if there isn't).
 * @param row a n-array of doubles, corresponding to a row in the original matrix.
 * @param n the dimension of the original matrix / the length of the given row.
 * @return a reference to the head of the new list (a nodeRef type variable).
 */
nodeRef row_to_list(double const *row, int n) {
    register nodeRef head = NULL, tail = NULL;
    register int i;
    register int nnz = 0;
    for (i = 0; i < n; ++i) {
        if (row[i] != 0) {
            if (head == NULL) {
                head = tail = malloc(sizeof(node));
            } else {
                tail->next = malloc(sizeof(node));
                tail = tail->next;
            }
            tail->value = row[i];
            tail->colind = i;
            tail->next = NULL;
            ++nnz;
        }
    }
    return head;
}

/**
 * Prints a given list, for debugging purposes.
 * This function is not necessary.
 * @param head a reference to the first item in the list.
 */
void print_list(nodeRef head) {
    if (head == NULL || head == 0)
        printf("NULL\n");
    else {
        printf("%f --> ", head->value);
        print_list(head->next);
    }
}

/**
 * This function removes each node in the given list,
 * and frees-up any memory resource that has been dynamically
 * allocated to produce the it.
 * @param list_head a reference to the first item in the list.
 */
void empty_list(nodeRef list_head) {
    if (list_head != NULL) {
        empty_list(list_head->next);
        free(list_head);
    }
}

/* linked list implementation ends here */

typedef struct matrix_pointers {
    int *rowptr;
    int *colind;
    double *values;
    int nextValueIndex;
} matrix_pointers;

/* function declarations. to be implemented later. */
void array_add_row(struct _spmat *A, const double *row, int i);

void array_free(struct _spmat *A);

void array_mult(const struct _spmat *A, const double *v, double *result);

void list_add_row(struct _spmat *A, const double *row, int i);

void list_free(struct _spmat *A);

void list_mult(const struct _spmat *A, const double *v, double *result);

/**
 * Initialize a new array-based sparse matrix.
 * The private field is used to store an implementation-specific struct.
 * @param n the dimension of the matrix.
 * @param nnz the number of non-zero elements in the matrix.
 * @return a pointer to a sparse matrix structure, implemented by arrays.
 */
spmat *spmat_allocate_array(int n, int nnz) {
    register spmat *mat = malloc(sizeof(spmat));
    register matrix_pointers *pointers = malloc(sizeof(matrix_pointers));
    assertMemoryAllocation(mat);
    assertMemoryAllocation(pointers);
    pointers->rowptr = (int *) calloc(n + 1, sizeof(int));
    assertMemoryAllocation(pointers->rowptr);
    pointers->colind = (int *) calloc(nnz, sizeof(int));
    assertMemoryAllocation(pointers->colind);
    pointers->values = (double *) calloc(nnz, sizeof(double));
    assertMemoryAllocation(pointers->values);
    (pointers->rowptr)[n] = nnz; /* This is where we keep the number of non-zero items is the spmat */
    pointers->nextValueIndex = 0;

    mat->n = n;
    mat->add_row = array_add_row;
    mat->free = array_free;
    mat->mult = array_mult;
    /* private field holds a pointer to a struct (using casting) */
    mat->private = pointers;
    return mat;
}

/**
 * Given an array-implemented sparse matrix,
 * this function inserts a given row as the i'th row.
 * @param A a pointer to the sparse matrix.
 * @param row a pointer to the row base address.
 * @param i the inserted row's index.
 */
void array_add_row(struct _spmat *A, const double *row, int i) {
    register matrix_pointers *pointers = (matrix_pointers *) (A->private);
    register int *rowptr = pointers->rowptr;
    register int *colind = pointers->colind;
    register double *values = pointers->values;
    register int n = A->n;
    register int j = 0;
    rowptr[i] = pointers->nextValueIndex;
    for (j = 0; j < n; j++) {
        if (row[j] != 0) {
            values[pointers->nextValueIndex] = row[j];
            colind[pointers->nextValueIndex] = j;
            pointers->nextValueIndex++;
        }
    }
}

/**
 * Frees up every resource that has been dynamically allocated
 * by an array-based sparse matrix.
 * @param A a pointer to the array-based sparse matrix.
 */
void array_free(struct _spmat *A) {
    register matrix_pointers *pointers = (matrix_pointers *) (A->private);
    free(pointers->rowptr);
    free(pointers->colind);
    free(pointers->values);
    free(pointers);
    free(A);
}

/**
 * Multiplies an array-based sparse matrix by a given vector.
 * Saves the result to a new vector.
 * @param A an array-based sparse matrix, of size n by n, to be multiplied.
 * @param v a column vector of size n to be multiplied.
 * @param result a new empty or arbitrary vector of size n, to be filled with the multiplication result.
 */
void array_mult(const struct _spmat *A, const double *v, double *result) {
    register int currentInd;
    register int n = A->n;
    register matrix_pointers *pointers = (matrix_pointers *) (A->private);
    register int *rowptr = pointers->rowptr;
    register int *colind = pointers->colind;
    register double *values = pointers->values;
    register int i;

    for (i = 0; i < n; ++i) {
        if (rowptr[i] == rowptr[i + 1]) /* This condition is met iff the i'th row is all zeros */
            result[i] = 0;
        else {
            double value = 0;
            for (currentInd = rowptr[i]; currentInd < rowptr[i + 1]; currentInd++) {
                value += values[currentInd] * v[colind[currentInd]];
            }
            result[i] = value;
        }
    }
}

/**
 * Initialize a new list-based sparse matrix.
 * The private field is used to store an array of lists,
 * corresponding to each row in the original matrix.
 * @param n the dimension of the matrix.
 * @return a pointer to a sparse matrix structure, implemented by lists.
 */
spmat *spmat_allocate_list(int n) {
    register spmat *mat = malloc(sizeof(spmat));
    register nodeRef *row_lists = (nodeRef *) malloc(n * sizeof(nodeRef));
    assertMemoryAllocation(mat);
    assertMemoryAllocation(row_lists);
    mat->n = n;
    mat->add_row = list_add_row;
    mat->free = list_free;
    mat->mult = list_mult;
    /* private field holds an array of row lists */
    mat->private = row_lists;
    return mat;
}

/**
 * Given an lists-implemented sparse matrix,
 * this function inserts a given row as the i'th row.
 * @param A a pointer to the sparse matrix.
 * @param row a pointer to the row base address.
 * @param i the inserted row's index.
 */
void list_add_row(struct _spmat *A, const double *row, int i) {
    register int n = A->n;
    register nodeRef list_head = row_to_list(row, n);
    register nodeRef *row_lists = (nodeRef *) A->private;
    row_lists[i] = list_head;
}

/**
 * Frees up every resource that has been dynamically allocated
 * by a lists-based sparse matrix.
 * @param A a pointer to the array-based sparse matrix.
 */
void list_free(struct _spmat *A) {
    register nodeRef *row_lists;
    register int i;
    assertMemoryAllocation(A);
    row_lists = (nodeRef *) A->private;
    for (i = 0; i < A->n; ++i)
        empty_list(row_lists[i]);
    free(row_lists);
    free(A);
}

/**
 * Multiplies a lists-based sparse matrix by a given vector.
 * Saves the result to a new vector.
 * @param A a lists-based sparse matrix, of size n by n, to be multiplied.
 * @param v a column vector of size n to be multiplied.
 * @param result a new empty or arbitrary vector of size n, to be filled with the multiplication result.
 */
void list_mult(const struct _spmat *A, const double *v, double *result) {
    register int i, j;
    register nodeRef currElem;
    register double sum;
    register nodeRef *row_lists = (nodeRef *) A->private;
    for (i = 0; i < A->n; ++i) {
        sum = 0;
        for (currElem = row_lists[i]; currElem != NULL; currElem = currElem->next) {
            j = currElem->colind;
            sum += v[j] * currElem->value;
        }
        result[i] = sum;
    }
}

double spmatValuesSum(spmat *spm) {
    int i;
    nodeRef *rows, node;
    double sum = 0;
    rows = (nodeRef *) spm->private;
    for (i = 0; i < spm->n; i++) {
        node = rows[i];
        while (node != NULL) {
            sum += node->value;
            node = node->next;
        }
    }
    return sum;
}


/**
 * Generate a random double
 * @param low inclusive
 * @param high inclusive
 * @return random number
 */
double drand(double low, double high) {
    return ((double) rand() * (high - low)) / (double) RAND_MAX + low;
}

/**
 * Generate a random symmetric sparse matrix
 * @param n matrix of size nxn
 * @param percent probability of non-zero values
 * @param mat regular matrix representation, should be allocated
 */
spmat *generateRandomSymSpmat(int n, double percent, Matrix *mat) {
    int i, j;
    double randNum;
    spmat *spm = spmat_allocate_list(n);
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i < j) {
                randNum = drand(0, 100);
                if (randNum <= percent) {
                    setVal(mat, i, j, 1);
                } else {
                    setVal(mat, i, j, 0);
                }
                setVal(mat, j, i, readVal(mat, i, j));
            } else if (i == j) {
                setVal(mat, i, j, 0);
            }
        }
        spm->add_row(spm, mat->values[i], i);
    }

    return spm;
}

/**
 * Print sparse matrix
 * @param spm
 */
void printSpmat(spmat *spm) {
    int i, j, col;
    nodeRef *rows = (nodeRef *) spm->private;
    for (i = 0; i < spm->n; i++) {
        nodeRef node = rows[i];
        for (j = 0; j < spm->n; j++) {
            col = node != NULL ? node->colind : spm->n;
            if (j < col) {
                printf("%d ", 0);
            } else {
                printf("%d ", (int) node->value);
                node = node->next;
            }
        }
        printf("\n");
    }
}

