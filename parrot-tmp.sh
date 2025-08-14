#!/bin/bash

# This script is used to run the Parrot program in the provided directory.
# Change the directory name to the Parrot directory. (Where is located the bin directory)
# Link this script to /usr/local/bin/parrot to run it from anywhere.

cd PARROT_DIRECTORY || exit 1
PARROT_DIRECTORY/bin/parrot "$@"