#!/usr/bin/env python3
"""Shuffle input file defining line N as initial one."""
import argparse
import sys

ALLOWED = {"1", "0", "\n", " "}


def parse_args():
    """Parse and check args."""
    app = argparse.ArgumentParser()
    app.add_argument("input_file", help="Input file to shuffle.")
    app.add_argument("line", type=int, help="Number of line to make it initial one (0 - based)")
    app.add_argument("--output", "-o", default="stdout", help="Output, default stdout.")
    app.add_argument("--trim", "-t", action="store_true", dest="trim",
                     help="Trim the dataset (remove dupl strings or 0/1 - only columns.")

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
    if args.line >= len(input_arr):
        sys.exit("Error! Line {} (0-based) doesn't exist!".format(args.line))
    # TODO: finish this

if __name__ == "__main__":
    main()
