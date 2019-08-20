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

echo "Running test 2, should return True"
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

echo "Testing python part"
echo "Install requirements.txt"
pip3 install -r requirements.txt
if [[ $? -ne 0 ]]
then
    echo "Pip3 install failed"
    exit 1
else
    echo "pip install successful!"
fi

echo "Generate input"
./generate_input.py 50 20 20 10 test
if [[ $? -ne 0 ]]
then
    echo "Input generator failed!"
    exit 1
else
    echo "Generate successful!"
fi

echo "Testing cleanup"
./generate_input.py clean
if [[ $? -ne 0 ]]
then
    echo "Input clean failed!"
    exit 1
else
    echo "Clean successful!"
fi
