#!/bin/sh

if [ -z "$1" ] || [ -z "$2" ] || [ $# -ne 2 ];
then
    echo
    echo Usage:   $0  current-version previous-version
    echo
    echo Example: $0  2.18  2.17
    echo
    exit 0
fi

#  --gpg-key-id 1b77fc09 ...  cepek@gnu.org
#
./announce-gen --package-name gama \
               --release-type stable \
               --current-version $1 \
               --previous-version $2 \
               --gpg-key-id c6e1824e0180b85f31b06b6acb6ce60d1b77fc09 \
               --url https://ftp.gnu.org/gnu/gama \
               --news NEWS
