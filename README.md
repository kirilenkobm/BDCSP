
# BDCSP - Binary, Decisional, Closest String Problem version

[![pipeline status](https://gitlab.com/kirilenkobm/bdcsp/badges/master/pipeline.svg)](https://gitlab.com/kirilenkobm/bdcsp/commits/master)

## What is the problem

Binary decisional closest string problem definition:

- Given a set of strings S = {s1, s2, s3, ... sk} of length L over the alphabet {0, 1}
- Also given a parameter K
- Question: is there a string sc such that d(s, si) <= K for all s ∈ S?

Where d(s1, s2) is a Humming distance between stings s1 and s2.

This problem considered to be NP-complete.

Here, I suggest a polynomial approximation for this task with precision and performance measurements.

The algorithm is explained in detail in the following article:

<https://example.com>

Briefly, the suggested algorithm solves the problem in:

``` math
O(W^2 * H^2 * W * u)
```

Where:

- W - string length
- H - number of strings
- u - user-defined multiplicator, default 1

in the __worst__ case (K ~ 33% of string length).

To achieve this, I developed some ideas, published in "Fixed-Parameter Algorithms for Closest String and Related Problems" by Jens Gramm, Rolf Niedermeier, and Peter Rossmanith3 (Algorithmica, September 2003, Volume 37, Issue 1, pp 25–42) and added a couple of mine. Also, using the binary alphabet simplifies the search a lot.

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

To run tests/*.ipynb install requirements first:

```shell
pip install -r requirements.txt
```

To generate input files:

```shell
make rnd
./generate_input
```

To reproduce testing dataset:

```shell
./repeat_dataset.sh
```

Will compile random datasets generator in C

Program usage:

```txt
Usage: ./CSP [input file] [k] [-v] [-p]
[input file]: text file containing input or stdin
[k]: minimal distance to check, positive integer number
[-v] <number 0 .. 3>: enable verosity mode, set logging level from 1 to 3, 0 - nothing
[-h]: show this message
[-V]: show version
[-p]: show patterns
[-nr]/[--nr]: you promise there are no repetative strings (not recommended) =)
[-r]: render initial state (not recommended on big datasets)
[-f]: optimize first line
[-s]: sanity checks, just check the input correctness and quit
[-sr]/[--sr] <filaname>: save final render to file
[-z]: turn on magic mode
[-t]: transpose dataset
```

For better (in average) results, add -f and -z options.

Also, results might be improved with:

```shell
../call_for_each_line.py [input_file] [k]
```

Warning - takes str_num more operations!

In some cases, this also might be useful:

```shell
./iterative_call.py
```

In this case the program runs several times using the best state of the previous program.

## Contents

- src/ - C source code, the algorithm implementation itself:
  - CSP.c/h - enrty point
  - read_input.c/h - read and check input
  - patterns.c/h - patterns-related operations
  - arrstuff.c/h - operations with arrays
  - render.c/h - hard to say
  - traverse.c/h - core algorithm
  - generate_dataset.c - generate dataset for BDCSP
- tests/ - folder for test files generated by generate_input.py
  - input_files/ - folder for input datasets
  - answers/ - folder with answers for each dataset
- metrics - folder for jupyter notebooks to evaluate tool precision and rerformance
- repeat_dataset.sh - script to reproduce dataset I used for evaluation
- Makefile and Win_make.bat - build instructions for Linux/MacOS and Windows
- run_tests.sh - shell script to check that everything builds and returns correct results for sample data
- shuffle_input.py - script to shuffle input making line N first
- requirements.txt - libraries required for performance tests and input generation in python
- Minor stuff:
  - LICENSE
  - .gitignore
  - .gitlab-ci.yml
  - CHANGELOG
  - README.md =)
