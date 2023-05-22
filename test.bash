#!/bin/bash
# List all directories inside test_cases folder

set +e  # Turn off errexit option
for dir in test_cases/*/; do
    # Get the directory name without the trailing slash
    dir_name=$(basename "$dir")

    # Run the compiler program for the directory
    ./compiler.exe "test_cases/$dir_name/$dir_name.cpp" "test_cases/$dir_name"
done