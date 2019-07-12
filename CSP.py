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

CHARS = {"1", "0"}


def parse_args():
    """Parse args and check if everything is correct."""
    app = argparse.ArgumentParser()
    app.add_argument("input_file", help="File containing strings.")
    app.add_argument("k", type=int, help="K to test.")
    app.add_argument("--verbose", "-v", action="store_true", dest="verbose")
    app.add_argument("--show_time", "-t", action="store_true", dest="show_time")
    args = app.parse_args()
    if args.k < 1:
        sys.exit("K must be >= 1")
    return args


def eprint(msg, end="\n"):
    """Print to stderr."""
    sys.stderr.write(msg + end)


def read_strings(input_file, v=False):
    """Read and check input stings."""
    with open(input_file, "r") as f:
        strings = [l[:-1] for l in f.readlines() if l != "\n"]
    # check data size
    str_num = len(strings)
    if str_num < 2:
        sys.exit("Error: at least two input strings required!")
    str_len = len(strings[0])
    if str_len < 2:
        sys.exit("Error: string length must be >= 2!")
    if any(len(strings[i]) != str_len for i in range(str_num)):
        sys.exit("Error: strings must have the same lenght!")
    if v:
        eprint("Input data contains {} strings of length {}".format(str_num, str_len))
    all_characters = set(flatten(strings))
    if all_characters.difference(CHARS):
        sys.exit("Error: Allowed characters are 1 and 0!")
    # ok, let's convert them into an arrray
    strings_int = [[int(c) for c in s] for s in strings]
    return strings_int, str_num, str_len


def flatten(lst):
    """Flatten a list of lists into a list."""
    return [item for sublist in lst for item in sublist]


def invert_pattern(pattern):
    """Return NOT pattern."""
    return [0 if v == 1 else 1 for v in pattern]


def extract_patterns(strings, str_num, str_len):
    """Extract patterns from the strings."""
    columns = [[s[n] for s in strings] for n in range(str_len)]
    actual_col_num = str_len
    id_to_pattern = []
    pattern_id_to_positions = {}
    pattern_to_positions = defaultdict(list)
    for num, column in enumerate(columns):
        if len(set(column)) == 1:
            actual_col_num -= 1
            continue
        ones_pattern = tuple(column)
        zeros_pattern = tuple(invert_pattern(ones_pattern))
        # two patterns per position
        pattern_to_positions[ones_pattern].append(num)
        pattern_to_positions[zeros_pattern].append(num)
    patterns_sorted = sorted([p for p in pattern_to_positions.keys()],
                             key=lambda x: sum(x),
                             reverse=True)
    for num, pattern in enumerate(patterns_sorted):
        id_to_pattern.append(pattern)
        pattern_id_to_positions[num] = pattern_to_positions[pattern]
    return id_to_pattern, pattern_id_to_positions, actual_col_num


def reformate_pattern_id_to_pos(pat_id_to_pos_dct):
    """Make a list instead of dict."""
    max_key = max(pat_id_to_pos_dct.keys())
    pat_id_to_pos = [[] for _ in range(max_key + 1)]
    positions_to_patterns = defaultdict(list)
    for pat_id in range(max_key + 1):
        pos_ = pat_id_to_pos_dct[pat_id]
        pat_id_to_pos[pat_id] = pos_
        for p_ in pos_:
            positions_to_patterns[p_].append(pat_id)
    all_positions = sorted(positions_to_patterns.keys())
    pos_to_pat_ids = [tuple() for _ in range(max(all_positions) + 1)]
    for pos_ in all_positions:
        pat_ids = tuple(sorted(positions_to_patterns[pos_]))
        pos_to_pat_ids[pos_] = pat_ids
    return pat_id_to_pos, pos_to_pat_ids, all_positions


def main():
    """Enrty point."""
    t0 = dt.now()
    try:  # try to find the C lib
        LIBS_DIR = os.path.join(os.getcwd(), "libs")
        CSP_LIB_PATH = os.path.join(LIBS_DIR, "CSP.so")
        CSP_SLIB = ctypes.CDLL(CSP_LIB_PATH)
        # arguments:
        CSP_SLIB.solve_CSP.argtypes = [ctypes.c_uint32,  # int 5 times
                                       ctypes.c_uint32,
                                       ctypes.c_uint32,
                                       ctypes.c_uint32,
                                       ctypes.c_uint32,  # int* 6 times
                                       ctypes.POINTER(ctypes.c_uint8),
                                       ctypes.POINTER(ctypes.c_int),
                                       ctypes.POINTER(ctypes.c_int),
                                       ctypes.POINTER(ctypes.c_int),
                                       ctypes.POINTER(ctypes.c_int),
                                       ctypes.POINTER(ctypes.c_int)]
        # returns bool
        CSP_SLIB.solve_CSP.restype = ctypes.c_bool
    except OSError:  # not found --> call make then and exit
        eprint("Error: shared libs not found")
        eprint("Calling make...")
        os.mkdir("libs") if not os.path.isdir("libs") else None
        rc = subprocess.call("make", shell=True)
        eprint("Make failed") if rc != 0 else eprint("Make successful")
        sys.exit("Try to call again.")
    # read and check input
    args = parse_args()
    in_strings, str_num, str_len = read_strings(args.input_file, args.verbose)
    # pre-process input
    id_to_pattern, patttern_id_to_positions_raw, act_col_num \
         = extract_patterns(in_strings, str_num, str_len)
    if act_col_num == 0:
        # nothing to cover
        print("All sequences are the same")
        print("Answer is:\ntrue")
        sys.exit(0)
    elif (act_col_num - args.k) <= 0:
        print("Nothing to cover at this K")
        print("Answer is:\ntrue")
        sys.exit(0)
    pattern_id_to_positions, positions_to_pat_ids, all_positions \
        = reformate_pattern_id_to_pos(patttern_id_to_positions_raw)
    # put this stuff into C function
    patterns_num = len(id_to_pattern)
    # pattern lenght is the str_num
    patterns_array = flatten(id_to_pattern)
    c_patterns_array = (ctypes.c_uint8 * (patterns_num * str_num + 1))()
    c_patterns_array[:-1] = patterns_array
    # patterns to positions
    pat_to_pos_array = flatten(pattern_id_to_positions)
    pat_to_pos_num = [len(x) for x in pattern_id_to_positions]
    pat_to_pos_arr_size = len(pat_to_pos_array)
    c_pat_to_pos_arr = (ctypes.c_int * (pat_to_pos_arr_size + 1))()
    c_pat_to_pos_arr[:-1] = pat_to_pos_array
    c_pat_to_pos_num = (ctypes.c_int * (patterns_num + 1))()
    c_pat_to_pos_num[:-1] = pat_to_pos_num
    pointer = 0
    pat_to_pos_starts = []
    for pos_ in pattern_id_to_positions:
        pat_to_pos_starts.append(pointer)
        pointer += len(pos_)
    c_pat_to_pos_starts = (ctypes.c_int * (patterns_num + 1))()
    c_pat_to_pos_starts[:-1] = pat_to_pos_starts
    # positions to patterns
    pos_to_patterns_array = flatten(positions_to_pat_ids)
    positions_num = len(all_positions)
    c_pos_to_pat_array = (ctypes.c_int * (positions_num * 2 + 1))()
    c_pos_to_pat_array[:-1] = pos_to_patterns_array
    # positions themselves
    c_pos_array = (ctypes.c_int * (positions_num + 1))()
    c_pos_array[:-1] = all_positions
    # single values
    c_str_num = ctypes.c_uint32(str_num)
    c_str_len = ctypes.c_uint32(act_col_num)
    c_k_ = ctypes.c_uint32(args.k)
    c_pos_num = ctypes.c_uint32(positions_num)
    c_pat_num = ctypes.c_uint32(patterns_num)
    # call the solver finally
    answer = CSP_SLIB.solve_CSP(c_str_num,  # int
                                c_str_len,  # int
                                c_k_,  # int
                                c_pat_num,  # int
                                c_pos_num,  # int
                                c_patterns_array,  # int*
                                c_pat_to_pos_arr,  # int*
                                c_pat_to_pos_num,  # int*
                                c_pat_to_pos_starts,  # int*
                                c_pos_array,
                                c_pos_to_pat_array,  # int*
    )
    print("# The answer is:\n{}".format(answer))
    if args.show_time:
        print("Elapsed: {}".format(dt.now() - t0))


if __name__ == "__main__":
    main()
