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

During the process of loading and converting a test file into a `testGraph` object, the program might print an error and exit if the file formatting is invalid. Every `' '` and every `'\n'` should be placed correctly, according to the instructions above.

## Example

Denote by `n` the number of vertices. Denote by `A[i][j]` the $(i,j)$ entry of the $n \times n$ adjacency matrix. Denote by `k` the number of groups the graph should be divided into. In this example we assume `k == 3`.

```c
n
A[0][0] A[0][1] ... A[0][n-1]
A[1][0] A[1][1] ... A[1][n-1]
... ... ... ... ...  ...  ...
A[n-1][0] A[n-1][1] ... A[n-1][n-1]
k
0 1 2 ... i
i+1 i+2 ... j
j+1 j+2 ... n
```

Notice that the number of vertices in each group is not known in advance. So `i` and `j`  here are arbitrary.