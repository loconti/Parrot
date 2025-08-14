#!/bin/bash7

# This script is used to create a link for the Parrot program.
# Usage: parrot-link <file_path>

if [ ! -d "$(pwd)/Files" ]; then
    echo "Creating parrot files folder at $(pwd)/Files"
    mkdir -p "$(pwd)/Files"
fi

if ! ln -s "$1" "$(parrot --folder-path)$(basename "$1")";
then 
    echo "Error: could not create link to $1."
    echo "Usage: $0 <file_path>"
    exit 1
fi