#!/usr/bin/env bash
# repeat datasets for evaluation
mydir="${0%/*}"
gen_in="$mydir/generate_input"

# just delete all generated files
eval "rm -rf tests/input_files/*"
echo "cleaned"

# make generate_input if not exists
if [ ! - f generate_input ];
then
    eval "make rnd"
fi

# tune this
str_lens=(100 100 500 500)
str_nums=(25 50 175 250)
mult=(1 1 5 5)  # to change K accordingly
range=4  # number of subsets

# maybe this
raw_ks=(15 25 35 50 75)
raw_ks_max_i=4
repl=100  # number of replicates

for step in `seq 0 $((range - 1))`
do
    str_len="${str_lens[$step]}"
    str_num="${str_nums[$step]}"
    mul="${mult[$step]}"
    echo "creating set of str_len $str_len and str_num $str_num"
    for k_ in `seq 0 $raw_ks_max_i`
    do
        raw_k="${raw_ks[$k_]}"
        this_k=$(($raw_k * mul))
        d_name="test_${str_len}_${str_num}_${this_k}"
        cmd="$gen_in $str_len $str_num $this_k $repl $d_name"
        # echo $cmd
        eval $cmd
    done
done

echo "Done."
