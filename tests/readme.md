Test File Format
================

-   The whole file consists of integers, split by spaces `‘ ‘` and line-breaks
    `‘\n’`.

-   The first line (1) contains the dimension n of the adjacency matrix.

-   Lines 2,...,n+1 represent the adjacency matrix. Namely, each row has
    n numbers, split by spaces, which represent the corresponding row
    entries of the adjacency matrix.

-   Line n+2 contains the number of communities (clusters) in the input
    graph, denoted by k.

-   Lines, (n+2)+1,...,(n+2)+k represent - each - a vertices set in the
    input graph. In each of these lines, the corresponding set members are
    divided by spaces.

It is unimportant to list the entries of the adjacency matrix in n separate
rows, though it makes the file more human-readable. If it is more convenient,
you may represent the matrix using one big row of n^2 entries.
