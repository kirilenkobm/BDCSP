#!/usr/bin/env python3
"""Create cluster jobs for performance evaluation."""
import os
import random

DATASETS_DIR = "tests/input_files"
JOBS_NUM = 100
JOBS_DIR = "perf_jobs"
RES_DIR = "perf_results"
COMBINED = "perf_comb_jobs.txt"


def parts(lst, n):
    """Split an iterable into parts with size n."""
    return [lst[i:i + n] for i in iter(range(0, len(lst), n))]


def main():
    """Entry point."""
    perf_datasets = [d for d in os.listdir(DATASETS_DIR) if d.startswith("perf")]
    commands = []
    for dataset in perf_datasets:
        dataset_data = dataset.split("_")
        str_len = int(dataset_data[1])
        k = int(dataset_data[3])
        dataset_path = os.path.join(DATASETS_DIR, dataset)
        dataset_files = os.listdir(dataset_path)
        files_paths = [os.path.join(dataset_path, df) for df in dataset_files]
        ds_cmds = ["{ " + "time ./CSP {0} {1}".format(fp, k) + "; } 2>&1" for fp in files_paths]
        commands.extend(ds_cmds)

    random.shuffle(commands)
    cmds_num = len(commands)
    job_size = int(cmds_num / JOBS_NUM)
    jobs = []

    for num, part in enumerate(parts(commands, job_size), 1):
        job_fname = f"part_{num}"
        job_path = os.path.join(JOBS_DIR, job_fname)
        with open(job_path, "w") as f:
            f.write("\n".join(part) + "\n")
        jobs.append(job_path)

    f = open(COMBINED, "w")
    for job in jobs:
        res_path = os.path.join(RES_DIR, os.path.basename(job))
        job_line = f"bash {job} > {res_path}"
        f.write(job_line + "\n")
    f.close()


if __name__ == "__main__":
    os.mkdir(JOBS_DIR) if not os.path.isdir(JOBS_DIR) else None
    os.mkdir(RES_DIR) if not os.path.isdir(RES_DIR) else None
    main()
