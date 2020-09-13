#include <stdio.h>
#include <stdlib.h>
#include "spmat.h"
#include "ErrorHandler.h"

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
                assertMemoryAllocation(tail);
                assertBooleanStatementIsTrue(head == tail);
            } else {
                tail->next = malloc(sizeof(node));
                assertMemoryAllocation(tail->next);
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

void list_add_row(struct _spmat *A, const double *row, int i);

void list_free(struct _spmat *A);

void list_mult(const struct _spmat *A, const double *v, double *result);


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
 * @param A a lists-based sparse matrix, of capacity n by n, to be multiplied.
 * @param v a column vector of capacity n to be multiplied.
 * @param result a new empty or arbitrary vector of capacity n, to be filled with the multiplication result.
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

double readSpmVal(spmat *spm, int r, int c) {
    nodeRef *rows = spm->private;
    nodeRef node = rows[r];
    while (node != NULL && node->colind <= c) {
        if (node->colind == c) {
            return 1;
        }
        node = node->next;
    }
    return 0;
}
