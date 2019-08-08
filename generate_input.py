#!/usr/bin/env python3
"""Script to generate input files for tests."""
import argparse
import os
import shutil
import sys
import subprocess
import numpy as np

TESTS_DIR = os.path.join(os.path.dirname(__file__), "tests")
INPUT_FILES_DIR = os.path.join(TESTS_DIR, "input_files")
ANSWER_FILES_DIR = os.path.join(TESTS_DIR, "answers")
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
    app.add_argument("--ones_fraction", "-o", type=float, default=0.5,
                     help="Float [0.0..1.0] - fraction of ones "
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
    elif args.ones_fraction < 0.0 or args.ones_fraction > 1.0:
        sys.exit("Error, ones_fraction must be in [0.0, 1.0]")
    return args


def main():
    """Entry point."""
    args = parse_args()
    ones_num = int(args.H * args.ones_fraction)
    zeros_num = args.H - ones_num
    # TODO: actually is not very floating-point errors resistsant
    origin_str = [1 for _ in range(ones_num)] + [0 for _ in range(zeros_num)]
    np.random.shuffle(origin_str)


if __name__ == "__main__":
    if len(sys.argv) == 2 and sys.argv[1] == "clean":
        # clean, but don't touch _readme.txt
        shutil.rmtree(INPUT_FILES_DIR) if os.path.isdir(INPUT_FILES_DIR) else None
        shutil.rmtree(ANSWER_FILES_DIR) if os.path.isdir(INPUT_FILES_DIR) else None
        sys.exit("Cleaned")
    os.mkdir(INPUT_FILES_DIR) if not os.path.isdir(INPUT_FILES_DIR) else None
    os.mkdir(ANSWER_FILES_DIR) if not os.path.isdir(ANSWER_FILES_DIR) else None
    main()
