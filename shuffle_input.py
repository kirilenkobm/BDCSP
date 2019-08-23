#!/usr/bin/env python3
"""Shuffle input file defining line N as initial one."""
import argparse
import random
import sys

ALLOWED = {"1", "0", "\n", " "}


def parse_args():
    """Parse and check args."""
    app = argparse.ArgumentParser()
    app.add_argument("input_file", help="Input file to shuffle.")
    app.add_argument("line", type=str, help="Number of line to make it initial one (0 - based). "
                                            "Write 'rnd' or -1 to randomize string order.")
    app.add_argument("--output", "-o", default="stdout", help="Output, default stdout.")
    app.add_argument("--cast", "-c", action="store_true", dest="cast",
                     help="Cast first line to 11111....11 line.")

    if len(sys.argv) == 1:
        app.print_help()
        sys.exit()
    args = app.parse_args()
    return args


def read_input(in_file):
    """Read and check input."""
    with open(in_file, "r") as f:
        content = f.read()
    if set(content).difference(ALLOWED):
        sys.exit("Error! Only these characters are allowed in input "
                 ": {}".format(ALLOWED))
    lines = [x.replace(" ", "") for x in content.split("\n") if x != ""]
    if len(set(len(x) for x in lines)) > 1:
        sys.exit("Error! Input lines should have the same length!")
    result = [[int(x) for x in line] for line in lines]
    return result


def revert(col):
    """Revert 10101 -> 01010."""
    return [1 - col[i] for i in range(len(col))]


def cast_to_ones(arr):
    """Cast first line to all ones."""
    columns = []
    str_num = len(arr)
    str_len = len(arr[0])
    for col_num in range(str_len):
        column = [arr[i][col_num] for i in range(str_num)]
        if column[0] == 1:
            columns.append(column)
            continue
        rev_col = revert(column)
        columns.append(rev_col)
    lines = []
    for line_num in range(str_num):
        line = [col[line_num] for col in columns]
        lines.append(line)
    return lines


def main():
    """Entry point."""
    args = parse_args()
    input_arr = read_input(args.input_file)
    try:
        l_num = -1 if args.line == "rnd" else int(args.line)
    except ValueError:
        sys.exit("Error! 'line' must be either a number or 'rnd' string")
    if l_num >= len(input_arr):
        sys.exit("Error! Line {} (0-based) doesn't exist!".format(args.line))
    elif l_num < -1:
        sys.exit("Error! Line num opt must be >= -1!")
    if l_num >= 0:
        temp = input_arr[0].copy()
        input_arr[0] = input_arr[l_num].copy()
        input_arr[l_num] = temp.copy()
    elif l_num == -1:
        # -1 means random
        random.shuffle(input_arr)
    else:
        raise NotImplementedError("It should never happen")
    if args.cast:
        input_arr = cast_to_ones(input_arr)
    f = open(args.output, "w") if args.output != "stdout" else sys.stdout
    for line in input_arr:
        out_line = "".join(str(x) for x in line)
        f.write(out_line + "\n")
    f.close() if  args.output != "stdout" else None

if __name__ == "__main__":
    main()
