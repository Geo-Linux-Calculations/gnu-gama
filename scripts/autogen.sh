#!/bin/sh

# eventually run autoreconf when it will work better
# autoreconf --force --install -I config

aclocal
autoheader
autoconf
automake -a -c -f
