#!usr/bin/env python3
"""Shuffle input file defining line N as initial one."""
import argparse
import sys

def parse_args():
    """Parse and check args."""
    app = argparse.ArgumentParser()
    app.add_argument("input_file", help="Input file to shuffle.")
    app.add_argument("line", type=int, help="Number of line to make it initial one (0 - based)")

    if len(sys.argv) == 1:
        app.print_help()
        sys.exit()
    args = app.parse_args()
    return args


def main():
    """Entry point."""
    args = parse_args()


if __name__ == "__main__":
    main()