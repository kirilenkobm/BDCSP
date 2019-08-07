#!/usr/bin/env python3
"""Script to generate input files for tests."""
import argparse
import os
import sys
import subprocess

TESTS_DIR = os.path.join(os.path.dirname(__file__), "tests")
README = os.path.join(TESTS_DIR, "_readme.txt")
UINT32_MAX = 4294967295


def parse_args():
    """Parse args and check."""
    app = argparse.ArgumentParser()
    app.add_argument("H", type=int, help="String lenght")
    app.add_argument("W", type=int, help="Strings number")
    app.add_argument("r", type=int, help="Replicates number, "
                                         "from 1 to 10000")
    app.add_argument("dataset_name", help="Dataset name")
    app.add_argument("--ones_percent", "-o", type=float, default=0.5,
                     help="Float [0.0..1.0] - percentage of ones "
                          "in the initial string")
    if len(sys.argv) == 1:
        app.print_help()
        sys.exit()
    args = app.parse_args()
    if args.H < 2 or args.H >= UINT32_MAX:
        sys.exit("Error, H must be in [2, {})".format(UINT32_MAX))
    elif args.W < 2 or args.W >= UINT32_MAX:
        sys.exit("Error, W must be in [2, {})".format(UINT32_MAX))
    elif args.r < 1 or args.r > 10000:
        sys.exit("Error, replicates nummber must be in [1, 10000]")
    elif args.ones_percent < 0.0 or args.ones_percent > 1.0:
        sys.exit("Error, ones_percent must be in [0.0, 1.0]")
    return args

def main():
    """Entry point."""
    args = parse_args()

if __name__ == "__main__":
    if len(sys.argv) == 2 and sys.argv[1] == "clean":
        # clean, but don't touch _readme.txt
        clean_cmd = "ls {}/* | grep -v {} | xargs rm".format(TESTS_DIR, README)
        subprocess.call(clean_cmd, shell=True)
        sys.exit("Cleaned")
    main()

