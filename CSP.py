#!/usr/bin/env python3
"""Closest string problem solution.

Decisional version on binary input.
"""
import argparse
import subprocess
import sys
import os
from collections import defaultdict
from datetime import datetime as dt
import ctypes

__author__ = "Bogdan Kirilenko"
__version__ = "0.03"
__email__ = "kirilenkobm@gmail.com"
__status__ = "Development"


class BDCspSolver:
    """Interface between python and C."""
    def __init__(self, input_strings, k):
        """Check lib and read input."""
        self.input_strings = input_strings
        self.str_num = len(input_strings)
        self.str_len = len(input_strings[0])
        self.k = k
        self.__check_lib()
        self.__extract_patterns()
        self.__reformate_pattern_id_to_pos()

    @staticmethod
    def flatten(lst):
        """Flatten a list of lists into a list."""
        return [item for sublist in lst for item in sublist]

    @staticmethod
    def invert_pattern(pattern):
        """Return NOT pattern."""
        return [0 if v == 1 else 1 for v in pattern]

    def __check_lib(self):
        """Check shared lib, compile if needed."""
        LIBS_DIR = os.path.join(os.getcwd(), "libs")
        CSP_LIB_PATH = os.path.join(LIBS_DIR, "CSP.so")
        if not os.path.isfile(CSP_LIB_PATH):
            eprint("Error: shared libs not found\nCalling make...")
            os.mkdir("libs") if not os.path.isdir("libs") else None
            rc = subprocess.call("make", shell=True)
            if rc != 0:
                raise RuntimeError("Make failed!")
        self.CSP_SLIB = ctypes.CDLL(CSP_LIB_PATH)
        # arguments:
        self.CSP_SLIB.solve_CSP.argtypes = [ctypes.c_uint32,  # int 5 times
                                            ctypes.c_uint32,
                                            ctypes.c_uint32,
                                            ctypes.c_uint32,
                                            ctypes.c_uint32,  # int* 6 times
                                            ctypes.POINTER(ctypes.c_uint8),
                                            ctypes.POINTER(ctypes.c_uint32),
                                            ctypes.POINTER(ctypes.c_uint32),
                                            ctypes.POINTER(ctypes.c_uint32),
                                            ctypes.POINTER(ctypes.c_uint32),
                                            ctypes.POINTER(ctypes.c_uint32)]
        # returns bool
        self.CSP_SLIB.solve_CSP.restype = ctypes.c_bool

    def __extract_patterns(self):
        """Extract patterns from the strings."""
        columns = [[s[n] for s in self.input_strings] for n in range(self.str_len)]
        self.actual_col_num = self.str_len
        self.id_to_pattern = []
        self.pattern_id_to_positions_raw = {}
        self.pattern_to_positions = defaultdict(list)
        for num, column in enumerate(columns):
            if len(set(column)) == 1:
                self.actual_col_num -= 1
                continue
            ones_pattern = tuple(column)
            zeros_pattern = tuple(self.invert_pattern(ones_pattern))
            # two patterns per position
            self.pattern_to_positions[ones_pattern].append(num)
            self.pattern_to_positions[zeros_pattern].append(num)
        patterns_sorted = sorted([p for p in self.pattern_to_positions.keys()],
                                key=lambda x: (sum(x), -1 * x.index(1)),
                                reverse=True)
        for num, pattern in enumerate(patterns_sorted):
            self.id_to_pattern.append(pattern)
            self.pattern_id_to_positions_raw[num] = self.pattern_to_positions[pattern]
        sys.exit("Answer is:\nTrue") if self.actual_col_num == 0 else None
        sys.exit("Answer is:\nTrue") if (self.actual_col_num - self.k) <= 0 else None
    
    def __reformate_pattern_id_to_pos(self):
        """Make a list instead of dict."""
        max_key = max(self.pattern_id_to_positions_raw.keys())
        self.pattern_id_to_positions = [[] for _ in range(max_key + 1)]
        positions_to_patterns_ = defaultdict(list)
        for pat_id in range(max_key + 1):
            pos_ = self.pattern_id_to_positions_raw[pat_id]
            self.pattern_id_to_positions[pat_id] = pos_
            for p_ in pos_:
                positions_to_patterns_[p_].append(pat_id)
        self.all_positions = sorted(positions_to_patterns_.keys())
        self.pos_to_pat_ids = [tuple() for _ in range(max(self.all_positions) + 1)]
        for pos_ in self.all_positions:
            pat_ids = tuple(sorted(positions_to_patterns_[pos_]))
            self.pos_to_pat_ids[pos_] = pat_ids
        self.patterns_num = len(self.id_to_pattern)
        self.positions_num = len(self.all_positions)

    def get_answer(self):
        """Solve CSP."""
        # pre-process input \put this stuff into C function
        # pattern lenght is the str_num
        patterns_array = self.flatten(self.id_to_pattern)
        c_patterns_array = (ctypes.c_uint8 * (self.patterns_num * self.str_num + 1))()
        c_patterns_array[:-1] = patterns_array
        # patterns to positions
        pat_to_pos_array = self.flatten(self.pattern_id_to_positions)
        pat_to_pos_num = [len(x) for x in self.pattern_id_to_positions]
        pat_to_pos_arr_size = len(pat_to_pos_array)
        c_pat_to_pos_arr = (ctypes.c_uint32 * (pat_to_pos_arr_size + 1))()
        c_pat_to_pos_arr[:-1] = pat_to_pos_array
        c_pat_to_pos_num = (ctypes.c_uint32 * (self.patterns_num + 1))()
        c_pat_to_pos_num[:-1] = pat_to_pos_num
        pointer = 0
        pat_to_pos_starts = []
        for pos_ in self.pattern_id_to_positions:
            pat_to_pos_starts.append(pointer)
            pointer += len(pos_)
        c_pat_to_pos_starts = (ctypes.c_uint32 * (self.patterns_num + 1))()
        c_pat_to_pos_starts[:-1] = pat_to_pos_starts
        # positions to patterns
        pos_to_patterns_array = self.flatten(self.pos_to_pat_ids)
        positions_num = len(self.all_positions)
        c_pos_to_pat_array = (ctypes.c_uint32 * (self.positions_num * 2 + 1))()
        c_pos_to_pat_array[:-1] = pos_to_patterns_array
        # positions themselves
        c_pos_array = (ctypes.c_uint32 * (positions_num + 1))()
        c_pos_array[:-1] = self.all_positions
        # single values
        c_str_num = ctypes.c_uint32(self.str_num)
        c_str_len = ctypes.c_uint32(self.actual_col_num)
        c_k_ = ctypes.c_uint32(self.k)
        c_pos_num = ctypes.c_uint32(self.positions_num)
        c_pat_num = ctypes.c_uint32(self.patterns_num)
        # call the solver finally
        answer = self.CSP_SLIB.solve_CSP(c_str_num,  # int
                                         c_str_len,  # int
                                         c_k_,  # int
                                         c_pat_num,  # int
                                         c_pos_num,  # int
                                         c_patterns_array,  # int*
                                         c_pat_to_pos_arr,  # int*
                                         c_pat_to_pos_num,  # int*
                                         c_pat_to_pos_starts,  # int*
                                         c_pos_array,
                                         c_pos_to_pat_array)  # int*
        return answer


def parse_args():
    """Parse args and check if everything is correct."""
    app = argparse.ArgumentParser()
    app.add_argument("input_file", help="File containing strings.")
    app.add_argument("k", type=int, help="K to test.")
    # TODO: verbosity more
    app.add_argument("--show_time", "-t", action="store_true", dest="show_time")
    args = app.parse_args()
    if args.k < 1:
        sys.exit("K must be >= 1")
    return args


def eprint(msg, end="\n"):
    """Print to stderr."""
    sys.stderr.write(msg + end)


def read_strings(input_file):
    """Read and check input stings."""
    allowed_chars = {"1", "0"}
    with open(input_file, "r") as f:
        strings = [l.rstrip() for l in f.readlines()]
    # check data size
    str_num = len(strings)
    sys.exit("Error: at least two input strings required!") if str_num < 2 else None
    sys.exit("Error: string length must be >= 2!") if str_num < 2 else None
    if any(len(strings[i]) != len(strings[0]) for i in range(str_num)):
        raise ValueError("Error: strings must have the same lenght!")
    all_characters = set(BDCspSolver.flatten(strings))
    if all_characters.difference(allowed_chars):
        raise ValueError("Error: Allowed characters are 1 and 0!")
    # ok, let's convert them into an arrray
    strings_int = [[int(c) for c in s] for s in strings]
    return strings_int


def main():
    """Enrty point."""
    t0 = dt.now()  # start time
    args = parse_args()  # parse and check args
    in_strings = read_strings(args.input_file)  # read and check strings
    solver = BDCspSolver(in_strings, args.k)  # prepare patterns and positions
    answer = solver.get_answer()  # call C library to solve the problem
    print("# The answer is:\n{}".format(answer))
    eprint("Elapsed: {}".format(dt.now() - t0)) if args.show_time else None


if __name__ == "__main__":
    main()
