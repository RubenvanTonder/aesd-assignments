#!/bin/bash

# Check if both arguuments are provided

if [ "$#" -ne 2 ]; then
	echo "Error: I need $0 <directory> <filename>"
	exit 1
fi

SEARCH_DIR="$1"
SEARCH_STR="$2"

if [ ! -d "$SEARCH_DIR" ]; then
	echo "Error: Directory $SEARCH_DIR does not exist"
	exit 1
fi

NUMFILES=$(find "$SEARCH_DIR" -type f | wc -l)
NUMLINES=$(grep -r "$SEARCH_STR" "$SEARCH_DIR" | wc -l)

echo "The number of files are $NUMFILES and the number of matching lines are $NUMLINES"
