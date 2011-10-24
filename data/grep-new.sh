#!/bin/sh

( cat ru.txt | awk '{print;print}'; cat "$1") | sed -e 's| *#.*||' | sort | uniq -u
