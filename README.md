
# BDCSP - Binary, Decisional, Closest String Problem version

## What is the problem

Given n length-m strings s1, s2, ..., sn, the closest string problem seeks for a new length-m string s such that d(s,si) ≤ k for all i, where d denotes the Hamming distance, and where k is as small as possible [1].
A decision problem version of the closest string problem, which is NP-complete, instead takes k as another input and questions whether there is a string within Hamming distance k of all the input strings. This problem is NP-complete even in the case, when the alphabet is binary [2].

Here, I suggest an efficient algorithm, that solves decisional version of the closest string problem over the binary alphabet in polynomial time. Likely, this is a good approximation. More details in the following article:

<https://example.com>

Briefly, the suggested algorithm solves the problem in:

``` math
O(N^3)
```

in the worst case.

## Usage

On linux/MacOS:

```shell
git clone git@gitlab.com:kirilenkobm/bdcsp.git
cd bdcsp/
make
./CSP.py input_data/test_1.txt 4
```

On windows:

```bat
git clone git@gitlab.com:kirilenkobm/bdcsp.git
cd bdcsp\
.\Win_make.bat
python .\CSP.py input_data\test_1.txt 4
```

--------------------------

## To write

* patterns concept
* complications on patterns concept
* why decisional and binary version
* estimated complexity

## References

[1] Lanctot J, Li M, Ma B, Wang S, Zhang L. Distinguishing string selection problems. Information and Computation. 2003. pp. 41–55.

[2] Frances M, Litman A. On covering problems of codes. Theoretical Computer Science. 1997;30(2):113–119.
