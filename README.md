# Community Structure in Networks

In complex networks, a network is said to have a **community structure** if the nodes of the network can be grouped into groups of nodes with dense connections internally, and sparser connections between groups. In this project, we implemented an algorithm for detecting community structures (also known as clusters) in a network.

For instance, if we look at the following graph, the most obvious way to divide the vertices into clusters would be `[0,1,2,3,4]` and `[5,6,7,8,9,10,11,12,13,14,15,16,17,18,19]`. But is it really that obvious? Notice that the picture artificially separates the vertices to help us *see* the clusters. Nevertheless, a visual representation is impractical and useless when dealing with bigger networks.

![Network for example](https://raw.githubusercontent.com/idodoron11/cproject-cluster/master/tests/graphs/graph4.svg?token=AA4TZMKH4AV5UZXSQN6SSSLAR2UXQ)

The first question we need to ask ourselves is **what property determines whether a group of vertices has a community structure?** For this, we use a measure called *modularity*, which aims to predict the likelihood of a vertices subset to be a community. We define the modularity of a group as the number of edges within that group, minus the expected number of edges in a random graph with the same degrees. Consequently, a given group of vertices in a network is considered a *community* if the number of edges within that group is significantly more than expected (by chance). This is the leading idea in a complex algorithm, that iteratively partitions the graph into sub-networks with high modularity.

## File Format

The input and output files are both binary files consisting *only* of integers.

### Input File

The first value represents the number of nodes in the network, $n=\left | V \right |$.

The second value represents the number of edges of the first node, i.e., $k_1$. It is followed by the $k_1$ indices of its neighbors, in increasing order.

The next value is $k_2$, followed by the $k_2$ indices of the neighbors of the second node, then $k_3$ and its $k_3$ neighbors, and so on until node $n$..

### Output File

The first value represents the number of groups in the division.

The second value represents the number of nodes in the first group, followed by the indices of the nodes in the group, in increasing order.

The next value is the number of nodes in the second group, followed by the indices of the nodes in the group, in increasing order.

The next value is the number of nodes in the second group, followed by the indices of the nodes in the group, then the number of nodes and indices of nodes in the third group, and so on until the last group.
