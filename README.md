Design Problem #4
-----------------

Compile
-------
gcc -o3 SetCover.c -o SetCover

Run
-------
./SetCover -f tests/s-k-20-35.txt

Set Run Limit 60s - will print out best solution so far on SIGINT signal
-------
timeout --signal=INT 60s ./SetCover -f ../tests/s-k-35-65.txt

You are to complete this design problem on your own without a partner. You should think about the problem from scratch and build something from the ground up that you fully understand. Make sure you understand the assigned problem thoroughly before you being programming. If you have a preference, you may use any programming language you wish, but if you don't have a compelling reason, you are encouraged to use C. (It would be appreciated if you wrote your code in such a way that I could run it on something I have access to.)

Remember, the reason we study algorithms is to find fast ways to solve problems. Analysis gives us a framework to compare different algorithms. but it washes away questions about constants, difficulty of implementation, and performance in practice. The design problems make us confront these issues. In this lab, you are to produce exact solutions as fast as possible.

As with previous design problems, you will submit a report of your results (as well as emailing me your source code). Your report should describe your algorithm (the key ideas behind your working program) and the optimizations you devised. You should report the best result your program produced for each test file, allowing a maximum execution time of one minute for problem instance. (For each file, report whether or not your program finished and the user CPU time as reported by the Linux utility "time".) We'll have a friendly competition in class based on these results.

The Problem

You are to design and implement an algorithm to solve the set cover problem. The problem takes as input a collection S of m subsets of the universal set U = {1, . . . , n}. The goal is to find the smallest subset S' ⊆ S such that the union of the subsets in S' is U. This is perhaps best understood through an example. Let U = {1, . . . , 12}, and S = {S1 , . . . , S6}, where

S1 = {1, 2, 3, 4, 5, 6},
S2 = {5, 6, 8, 9},
S3 = {1, 4, 7, 10},
S4 = {2, 5, 7, 8, 11},
S5 = {3, 6, 9, 12}, and
S6 = {10, 11}.
In this case, the minimum set cover consists of exactly three subsets, S' = {S3 , S4 , S5}.

Set cover arises whenever you try to efficiently acquire or represent items which have been packaged in a fixed set of lots. You want to obtain all the items while buying as few lots as possible. Finding a cover is easy, because you can simply buy one of each lot. However, if you find a small set cover you can avoid buying unnecessary lots. An interesting application of set cover is boolean logic minimization.

What is known about algorithms for set cover? The problem is NP-complete, meaning you will become famous if you find an algorithm with polynomial worst-case running time. Nevertheless, you should design and implement the most efficient algorithm you can. With a little reflection, a straightforward backtracking algorithm should be apparent: iterate through all O(2m) subsets of S, testing each (worst case O(n · m)) to see if it is a cover.

You should test your code on data files with a variety of sizes. Feel free to create your own test files to help debug your program. A collection of data files can be obtained by clicking here. Each file has a name like "s-X-10-20", meaning that the file contains a set of 20 subsets of special type X, with U = {1, . . . , 10}. (Don't worry about the meaning of "X" -- it is irrelevant for our purposes here.) In a combinatorially explosive problem such as this, adding one to the problem size can multiply the running time by n, so test on the smaller files first. After each round of changes, you'll want to verify that your program is still working correctly on the smaller data files. Incorrect programs will receive limited credit. A program will be deemed incorrect if it (a) terminates and reports a final answer that is not optimal, or (b) cannot produce at least one covering set (not necessarily minimal) for every test file within one minute.

Writing efficient programs is somewhat of an iterative process. Build your first solution so you can throw it away, and start it early enough so you can go through several iterations. It is highly recommended that you start with a simple backtracking approach first. Don't try to improve on it until you have something that works. It is possible to get a working program in about 100 lines of code. Don't forget to use the code optimizer on your compiler when you make the final run, to get a free 20% or so speedup. The system profiler tool, gprof, will help you tune your program. (If you don't know how to use it, this would be a good time to explore it.)

How might you optimize the performance of backtracking code? Instead of testing every subset of S, you should be able to develop a backtracking algorithm which prunes partial solutions when it can be determined that there is no way to complete a given collection of subsets with a lower cost than the best solution found so far. Consider a possible reordering of subsets so that the search might proceed more quickly. Starting with a good approximate solution may help achieve faster cutoffs. Moreover, there is room for cleverness in selecting data structures to minimize the time needed to test whether a given collection of subsets covers U.

Input

The first line of each input file specifies the size of the universal set (n), and the second line gives the number of subsets in S (m). Each subsequent line contains a list of the elements in the next subset, beginning with S1 and continuing through Sm.

Output

Your program must output the size of the best cover (the number of subsets it includes) on a line by itself, followed by a line for each subset included in the cover. This line begins with the subset index number (x for Sx) in parentheses, followed by a listing of the elements in that subset in the same order they occurred in the input.

Sample Input

For the example above, the input file would contain the following:

12
6
1 2 3 4 5 6
5 6 8 9
1 4 7 10
2 5 7 8 11
3 6 9 12
10 11

Sample Output

For the example above, your program should output the following:

3
(3) 1 4 7 10
(4) 2 5 7 8 11
(5) 3 6 9 12
