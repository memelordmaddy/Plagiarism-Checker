zero
1 470 290 0 0
one
1 487 357 0 0
two
1 487 295 0 0
three
1 464 562 0 0
four
1 473 562 0 0
five 
1 488 562 0 0

False negative
Hacked: zero, one, two, three, four, five

Implemented two different versions of the Kruskal's algorithm, one where edges
were added one by one based on minimum weight while avoiding creating cycles, 
and another by removing edges from the given graph, while ensuring connectivity.

Leetcode question: https://leetcode.com/problems/min-cost-to-connect-all-points?envType=problem-list-v2&envId=minimum-spanning-tree
NOTE:   The reverse kruskal algorithm gives correct output, but times out in a few 
        testcases. This is because the number of edges in the testcases becomes very large.
        