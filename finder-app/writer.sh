#!/bin/bash

# Check if both arguments are provided


if [ "$#" -ne 2 ]; then
	echo "Error: I need $0 <filename> <string>"
	exit 1
fi

FILENAME="$1"
STRING="$2"

DIR_PATH=$(dirname "$FILENAME")
mkdir -p "$DIR_PATH"

# WRite the string to the file
echo "Writing '$STRING' to '$FILENAME'..."
echo "$STRING" > "$FILENAME"
echo "Succesfully wrote to '$FILENAME'"
