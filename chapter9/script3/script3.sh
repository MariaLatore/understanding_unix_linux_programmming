#!/bin/sh
# script3 - shows how an environment variable is passed to commands
# TZ is time zone, affect things like date, and ls -1
#

echo "The time in Boston is"
    TZ=EST5EDT
    export TZ  # add TZ to the environment
    date
echo "The time in CHicago is"
    TZ=CST6CDT
    date
echo "The time in LA is"
    TZ=PST8PDT
    date

