#!/usr/bin/env bash
# repeat datasets for evaluation
./generate_input.py clean

./generate_input.py 100 50 15 50 test_100_50_15
./generate_input.py 100 50 25 50 test_100_50_25
./generate_input.py 100 50 50 50 test_100_50_50
./generate_input.py 100 50 75 50 test_100_50_75

./generate_input.py 1000 500 150 50 test_1000_500_150
./generate_input.py 1000 500 250 50 test_1000_500_250
./generate_input.py 1000 500 500 50 test_1000_500_500
./generate_input.py 1000 500 750 50 test_1000_500_750

echo "Done."
