#!/bin/bash

convert() {
python3 -c 'import sys, json
a = [ x.rstrip("\n") for x in sys.stdin.readlines() ]
json.dump(a, sys.stdout, indent=2)'  
}

#wget "https://users.cs.duke.edu/~ola/ap/linuxwords" \
#	-O words-linux.txt
#wget "https://raw.githubusercontent.com/first20hours/google-10000-english/master/google-10000-english.txt" \
#	-O words-google-10000-english.txt

convert < words-google-10000-english.txt > words-google-10000-english.json
convert < words-linux.txt                > words-linux.json




