#!/bin/bash

set +e  # Turn off errexit option
i=1

# List all directories inside test_cases folder
for dir in test_cases/*/; do
    # Get the directory name without the trailing slash
    dir_name=$(basename "$dir")

    # Run the compiler program for the directory
    ./compiler.exe "test_cases/$dir_name/"*.cpp "test_cases/$dir_name"

    # Print index and directory name
    echo  -e "------------------------------------\n"
    echo "Test case $i: $dir_name done"

    # Increment counter variable
    ((i++))
done

# ./compiler.exe "test_cases/$dir_name/$dir_name.cpp" "test_cases/$dir_name"