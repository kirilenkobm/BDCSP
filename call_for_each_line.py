#!/usr/bin/env python3
"""Python wrapper that calls CSP for each line separately.

Collects data for each line, chooses the best one.
"""
import argparse
import multiprocessing as mp
import subprocess
import sys
import os
import shutil
import string
import random

MY_DIR = os.path.dirname(__file__)
CSP_BIN = os.path.join(MY_DIR, "CSP")


def parse_args():
    """Read and check args."""
    app = argparse.ArgumentParser()
    app.add_argument("in_file", help="Input file.")
    app.add_argument("k", type=int, help="Distance to closest string")
    app.add_argument("-p", action="store_true", dest="p",
                     help="Just show commands (temp dir is not deleted)")
    if len(sys.argv) < 2:
        app.print_help()
        sys.exit(0)
    args = app.parse_args()
    return args


def rnd_string(n=10):
    """Generate a random string."""
    return ''.join(random.SystemRandom().choice(string.ascii_uppercase + string.digits) for _ in range(n))


def call_for_line(input_data, line_num, k_, tmp_dir, p=False):
    """Call CSP for a particular initial line."""
    # prepare the input file first
    in_data_copy = input_data[:]
    tmp = in_data_copy[0]
    in_data_copy[0] = in_data_copy[line_num]
    in_data_copy[line_num] = tmp
    tmp_filemame = rnd_string() + ".txt"
    tmp_path = os.path.join(tmp_dir, tmp_filemame)
    with open(tmp_path, "w") as f:
        f.write("\n".join(in_data_copy) + "\n")
    cmd = "{} {} {} -v 1".format(CSP_BIN, tmp_path, k_)
    if p:
        print(cmd)
    result = subprocess.check_output(cmd, shell=True).decode("utf-8")
    return result


def get_k(result_str, k_req, str_len):
    """Get K from the result."""
    res_data = result_str.split("\n")
    ans_ = res_data[-3]
    if ans_ == "True":
        return k_req
    # False
    if res_data[-5] == "# Cannot find initial move":
        return str_len
    cov_data = res_data[-5].split()
    exp = int(cov_data[-1])
    real = int(cov_data[-3])
    diff = exp - real
    return k_req + diff

def main():
    """Entry point."""
    args = parse_args()
    f = open(args.in_file, "r")
    lines = [line.rstrip("\n") for line in f.readlines()]
    f.close()
    str_num = len(lines)
    str_lens = set(len(l) for l in lines)
    if len(str_lens) > 1:
        sys.exit("Input must have lines of the same length")
    tmp_dir = "TEMP_{}".format(rnd_string())
    os.mkdir(tmp_dir) if not os.path.isdir(tmp_dir) else None

    pool = mp.Pool(mp.cpu_count())
    results =  [pool.apply(call_for_line, args=(lines, l_num, args.k, tmp_dir, args.p))
                for l_num in range(str_num)]
    if args.p:
        sys.exit(0)
    answers = [pool.apply(get_k, args=(r, args.k, str_len)) for r in results]
    ans_sort = sorted(answers)
    if ans_sort[0] == args.k:
        print("The answer is:\nTrue")
    else:
        print("The biggest K found: {}".format(ans_sort[0]))
        print("The answer is:\nFalse")
    shutil.rmtree(tmp_dir)


if __name__ == "__main__":
    main()
