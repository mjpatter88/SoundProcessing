#!/bin/bash

# This script simulates the pressing of a button on a telephone by generating the dtmf
# signal that will be generated.  It does this by calling "gensine" on the two appropriate
# frequencies and then mixing the two together for the final output.
#
# First command line argument is the label of the phone button pressed. (S - *, P - #)
# Second command line argument is the duration of the button pressed.

bit_size=8
sample_rate=8000
amplitude=100
delete_folder=0
legal_param=0

FREQ_ROWS=(697 770 852 941)
FREQ_COLS=(1209 1336 1477 1633)

if [ -z $1 ]; then
	echo "You must provide two parameters."
	exit 1
fi
if [ -z $2 ]; then
	echo "You must provide two parameters."
	exit 1
fi

if [ $1 = 1 ]; then
	frequency1=${FREQ_ROWS[0]}
	frequency2=${FREQ_COLS[0]}
	legal_param=1
fi
if [ $1 = 2 ]; then
	frequency1=${FREQ_ROWS[0]}
	frequency2=${FREQ_COLS[1]}
	legal_param=1
fi
if [ $1 = 3 ]; then
	frequency1=${FREQ_ROWS[0]}
	frequency2=${FREQ_COLS[2]}
	legal_param=1
fi
if [ $1 = A ]; then
	frequency1=${FREQ_ROWS[0]}
	frequency2=${FREQ_COLS[3]}
	legal_param=1
fi
if [ $1 = 4 ]; then
	frequency1=${FREQ_ROWS[1]}
	frequency2=${FREQ_COLS[0]}
	legal_param=1
fi
if [ $1 = 5 ]; then
	frequency1=${FREQ_ROWS[1]}
	frequency2=${FREQ_COLS[1]}
	legal_param=1
fi
if [ $1 = 6 ]; then
	frequency1=${FREQ_ROWS[1]}
	frequency2=${FREQ_COLS[2]}
	legal_param=1
fi
if [ $1 = B ]; then
	frequency1=${FREQ_ROWS[1]}
	frequency2=${FREQ_COLS[3]}
	legal_param=1
fi
if [ $1 = 7 ]; then
	frequency1=${FREQ_ROWS[2]}
	frequency2=${FREQ_COLS[0]}
	legal_param=1
fi
if [ $1 = 8 ]; then
	frequency1=${FREQ_ROWS[2]}
	frequency2=${FREQ_COLS[1]}
	legal_param=1
fi
if [ $1 = 9 ]; then
	frequency1=${FREQ_ROWS[2]}
	frequency2=${FREQ_COLS[2]}
	legal_param=1
fi
if [ $1 = C ]; then
	frequency1=${FREQ_ROWS[2]}
	frequency2=${FREQ_COLS[3]}
	legal_param=1
fi
if [ $1 = S ]; then
	frequency1=${FREQ_ROWS[3]}
	frequency2=${FREQ_COLS[0]}
	legal_param=1
fi
if [ $1 = 0 ]; then
	frequency1=${FREQ_ROWS[3]}
	frequency2=${FREQ_COLS[1]}
	legal_param=1
fi
if [ $1 = P ]; then
	frequency1=${FREQ_ROWS[3]}
	frequency2=${FREQ_COLS[2]}
	legal_param=1
fi
if [ $1 = D ]; then
	frequency1=${FREQ_ROWS[3]}
	frequency2=${FREQ_COLS[3]}
	legal_param=1
fi

#check that frequencies were set, meaning the input was valid
if [ $legal_param -eq 0 ]; then
	echo "You must enter a valid button."
	exit 1
fi

duration=$2
tempdir=tmp
if [ -d $tempdir ]; then
	echo ""
else
	mkdir $tempdir
	delete_folder=1
fi
temp1=$tempdir/temp_out1.raw
temp2=$tempdir/temp_out2.raw

./gensine MONO $frequency1 $bit_size $sample_rate $amplitude $duration > $temp1
./gensine MONO $frequency2 $bit_size $sample_rate $amplitude $duration > $temp2

#then call mix on those two files
./mix $temp1 1.0 $temp2 1.0

#then remove the two temp files
rm $temp1
rm $temp2

#remove the directory if it did not already exist
if [ $delete_folder -eq 1 ]; then
	rm -r $tempdir
fi