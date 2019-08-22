#!/usr/bin/env bash
# repeat datasets for evaluation
./generate_input.py clean
./generate_input.py 100 50 30 50 test_100_50_30

echo "Done."
