#!/usr/bin/env bash
printf "Started BDCSP tests\n"

declare -a arr=("make clean"
                "make"
                "./CSP _sample_input/test_0.txt 2"
                "./CSP _sample_input/test_0.txt 4"
                "./CSP _sample_input/test_2.txt 12"
                "pip3 install -r requirements.txt"
                "./generate_input.py 50 20 20 10 test"
                "./generate_input.py clean"
                )

for (( i = 0; i < ${#arr[@]} ; i++ )); do
    printf "\n**** Running: ${arr[$i]} *****\n\n"

    # Run each command in array 
    out=`${arr[$i]}`

    if [[ $? -ne 0 ]]
    then
        printf "${arr[$i]} FAILED\n"
        exit 1
    else
        printf "${arr[$i]} successful!\n"
    fi

    if [[ ( "$i" -eq 2 && $out == *False ) || \
         ( "$i" -eq 3 && $out = *True ) || \
         ( "$i" -eq 4 && $out = *True ) ]]
    then
        printf "Test $((i + 1)) returned correct answer\n"
    fi 

    if [[ ( "$i" -eq 2 && $out == *True ) || \
         ( "$i" -eq 3 && $out = *False ) || \
         ( "$i" -eq 4 && $out = *False ) ]]
    then
        printf "Test $((i + 1)) returned wrong answer\n"
        exit 1
    fi

    printf "Test $((i + 1)) out of 7\n"
done

exit 0
