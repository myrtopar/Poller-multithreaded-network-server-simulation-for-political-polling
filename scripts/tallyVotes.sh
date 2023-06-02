#!/bin/bash

tallyResultsFile=$1

# Check if the inputFile exists and has the appropriate permissions
if [ ! -f "inputFile.txt" ] || [ ! -r "inputFile.txt" ]; then
  echo "Error: inputFile does not exist or is not readable."
  exit 1
fi