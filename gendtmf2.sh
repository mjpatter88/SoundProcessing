#!/bin/bash

# This script simulates the pressing of a sequence of buttons on a telephone 
# by generating the dtmf signals with a given pause in between them.  It does this by
# using the "gendtmf" script and then using the "merge" program to combine the various
# button presses into one file. 
# 
# The first command line argument is the duration in ms of the pause between presses.
# The second command is the duration is seconds of the actual button presses
# The rest of the command line arguments are labels of the phone button pressed. (S - *, P - #)

if [ $# -lt 3 ]; then
	echo "You must provide at least three parameters."
	exit 1
fi

args=("$@")
delete_folder=0
tempdir=tmp
if [ -d $tempdir ]; then
	echo ""
else
	mkdir $tempdir
	delete_folder=1
fi

tempbase=$tempdir/temp_tone_

# loop through each input param starting at 2, for each value run the "gendtmf.sh" script
# with the duration in the second input param.
for (( i=2;i<$#; i++)); do
    ./gendtmf.sh ${args[${i}]} $2 > $tempbase$i.raw
    if [ $? -ne 0 ]; then
    		exit 1
    fi
done

# Next call the "merge.c" program with the pause length specified in the input param 1 and
# with each of the filenames from the previous part of the script.

for (( i=2;i<$#; i++)); do
	output="${output} $tempbase$i.raw"
done

./merge $1 $output

if [ $? -ne 0 ]; then
	exit 1
fi

# Remove the temp files and the temp directory if this created it
for (( i=2;i<$#; i++)); do
	rm -r $tempbase$i.raw
done

if [ $delete_folder -eq 1 ]; then
	rm -r $tempdir
fi