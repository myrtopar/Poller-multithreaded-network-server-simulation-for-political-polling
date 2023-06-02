#!/bin/bash

pollLog=$1
pollerResultsFile="pollerResultsFile.txt"

# Check if the pollLog exists and has the appropriate permissions
if [ ! -f "$pollLog" ] || [ ! -r "$pollLog" ]; then
  echo "Error: pollLog does not exist or is not readable."
  exit 1
fi

if [ -f "$pollerResultsFile" ]; then
  rm "$pollerResultsFile"
fi

if [ ! -f "politicalParties.txt" ] || [ ! -r "politicalParties.txt" ]; then
  echo "Error: politicalParties does not exist or is not readable."
  exit 1
fi

# create an array (like a python dictionary) that will contain pairs of (party -> votes)
declare -A voteCounts

# initialize the array so that every party (from politicalParties.txt) has 0 votes
while read line; do
  voteCounts["$line"]=0
done < "politicalParties.txt"

# sort the lines of the pollLog file using a key for sorting the fields 1, 2 (name, surname) and delete the duplicates (-u)
pollLogSorted="pollLogSorted.txt"
sort -u -k 1,2 "$pollLog" > "$pollLogSorted"

while read line; do
  party=$(echo "$line" | awk '{print $3}')
  voteCounts["$party"]=$((voteCounts["$party"] + 1))
done < "$pollLogSorted"

# Iterate over the array and print pairs to the pollerResultsFile
for party in "${!voteCounts[@]}"; do
  votes="${voteCounts[$party]}"
  echo "$party $votes" >> "$pollerResultsFile"
done