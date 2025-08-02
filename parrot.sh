#!/bin/bash

# This script is used to run the Parrot program in the provided directory.
# Change the directory name to the Parrot directory. (Where is located the bin directory)
# Link this script to /usr/local/bin/parrot to run it from anywhere.

cd /home/lore/Pro/Parrot || exit 1
/home/lore/Pro/Parrot/bin/parrot "$@"