#!/usr/bin/env python3
"""Call CSP several times use previous best result.

A way to obtein the most precise result.
"""
import argparse
import subprocess
import sys
import os
import random
import string

MAX_ITER = 3
TEMP_DIR = "_temp"
CMD_TEMPLATE = "./CSP {} {} --sr {} -z -f -v 1"
TRUE_ANS = "The answer is:\nTrue"
FALSE_ANS = "The answer is:\nFalse"


def rnd_string(n=10):
    """Generate a random string."""
    return ''.join(random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(n))


def parse_args():
    """Parse and check args."""
    app = argparse.ArgumentParser()
    app.add_argument("input", help="Input file")
    app.add_argument("k", help="K we are looking for")
    if len(sys.argv) < 3:
        app.print_help()
        sys.exit(0)
    args = app.parse_args()
    return args


def parse_cmd_out(cmd_out):
    """Get true of false."""
    lines = cmd_out.split("\n")
    if "True" in lines:
        return True
    else:
        return False


def main():
    """Entry point."""
    os.mkdir(TEMP_DIR) if not os.path.isdir(TEMP_DIR) else None
    args = parse_args()
    iter_input = [args.input]
    answer = False

    for iter_num in range(MAX_ITER):
        in_file = iter_input[iter_num]
        next_filename = "{}.txt".format(rnd_string())
        next_path = os.path.join(TEMP_DIR, next_filename)
        iter_input.append(next_path)
        cmd = CMD_TEMPLATE.format(in_file, args.k, next_path)
        print(cmd)
        cmd_out = subprocess.check_output(cmd, shell=True).decode("utf-8")
        # print(cmd_out)
        cmd_res = parse_cmd_out(cmd_out)
        if cmd_res:
            answer = True
            break
    # print answer
    if answer is True:
        print(TRUE_ANS)
    else:
        print(FALSE_ANS)
    # remove trash we left
    for f in iter_input[1: ]:
        os.remove(f) if os.path.isfile(f) else None


if __name__ == "__main__":
    main()
