#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Invalid number of arguments. Expected 2 arguments."
  exit 1
fi

filename=$1
numLines=$2

#-f is a file test operator in bash that checks if a file exists and is a regular file
if [ ! -f "$filename" ]; then
  echo "the file $filename does not exist."
  exit 1
fi

inputFile="inputFile.txt"
rm -f "$inputFile"

for ((i=1; i<=numLines; i++)); do
  randomName=$(cat /dev/urandom | tr -dc 'a-zA-Z' | head -c $((RANDOM%10+3)))
  randomSurname=$(cat /dev/urandom | tr -dc 'a-zA-Z' | head -c $((RANDOM%10+3)))
  randomParty=$(shuf -n 1 "$filename")
  echo "$randomName $randomSurname $randomParty" >> "$inputFile"
done
