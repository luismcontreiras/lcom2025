#!/bin/bash

# Script to stop, clean, build and run the LCOM project
# Usage: ./run_proj.sh

echo "Stopping any running LCOM project..."
lcom_stop proj

echo "Cleaning previous build..."
make clean

echo "Building project..."
make

# Check if make was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Running project..."
    lcom_run proj
else
    echo "Build failed! Please check the errors above."
    exit 1
fi
