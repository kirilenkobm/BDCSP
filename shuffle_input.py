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
                                            "Write 'rnd' or 0 to randomize string order.")
    app.add_argument("--output", "-o", default="stdout", help="Output, default stdout.")

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

def main():
    """Entry point."""
    args = parse_args()
    input_arr = read_input(args.input_file)
    try:
        l_num = 0 if args.line == "rnd" else int(args.line)
    except ValueError:
        sys.exit("Error! 'line' must be either a number or 'rnd' string")
    if l_num >= len(input_arr):
        sys.exit("Error! Line {} (0-based) doesn't exist!".format(args.line))
    if l_num != 0:
        temp = input_arr[0].copy()
        input_arr[0] = input_arr[args.line].copy()
        input_arr[args.line] = temp.copy()
    else:
        random.shuffle(input_arr)
    f = open(args.output, "w") if args.output != "stdout" else sys.stdout
    for line in input_arr:
        out_line = "".join(str(x) for x in line)
        f.write(out_line + "\n")
    f.close() if  args.output != "stdout" else None

if __name__ == "__main__":
    main()
