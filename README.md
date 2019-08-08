
# BDCSP - Binary, Decisional, Closest String Problem version

## What is the problem

What is the problem: [1]. Is also np complete in case of binary alphabet:[2]. Coming soon.

Here, I suggest an efficient algorithm, that solves decisional version of the closest string problem over the binary alphabet in polynomial time. Likely, this is a good approximation. More details in the following article:

<https://example.com>

Briefly, the suggested algorithm solves the problem in:

``` math
O(N^4)
```

in the worst case.

## Usage

On linux/MacOS:

```shell
git clone git@gitlab.com:kirilenkobm/bdcsp.git
cd bdcsp/
make
```

On windows:

```bat
git clone git@gitlab.com:kirilenkobm/bdcsp.git
cd bdcsp\
.\Win_make.bat
```

To run generate_inputs.py and tests.ipynb install requirements first:

```shell
pip install -r requirements.txt
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
