#!/bin/bash

if [ ! -d "$(pwd)/Files" ]; then
    echo "Creating parrot files folder at $(pwd)/Files"
    mkdir -p "$(pwd)/Files"
fi
if ! sed "s|PARROT_DIRECTORY|$(pwd)|g" "$(pwd)/parrot-tmp.sh" > "$(pwd)/parrot.sh";
then
    echo "Error: during sed command."
    echo "Installation failed."
    exit 1
fi


sudo ln -s "$(pwd)/parrot-link.sh" /usr/local/bin/parrot-link
sudo chmod +x /usr/local/bin/parrot-link
sudo ln -s "$(pwd)/parrot.sh" /usr/local/bin/parrot
sudo chmod +x /usr/local/bin/parrot
