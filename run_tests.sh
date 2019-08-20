#!/usr/bin/env bash
echo "Testing build"
make clean
make

if [[ $? -ne 0 ]]
then
    echo "Make failed"
    exit 1
else
    echo "Build successful!"
fi

# TODO: make for loop over the commands
echo "Running test 1, should return False"
test1=$(./CSP _sample_input/test_0.txt 2> /dev/null 1)

if [[ $? -ne 0 ]]
then
    echo "Test 1 returned != 0 exit code"
    exit 1
elif [[ $test1 = *False ]]
then
    echo "Test 1 - correct answer"
else
    echo "Test 1 - wrong answer"
    exit 1
fi

echo "Running test 1, should return True"
test2=$(./CSP _sample_input/test_0.txt 2> /dev/null 5)

if [[ $? -ne 0 ]]
then
    echo "Test 2 returned != 0 exit code"
    exit 1
elif [[ $test2 = *True ]]
then
    echo "Test 2 - correct answer"
else
    echo "Test 2 - wrong answer"
    exit 1
fi
