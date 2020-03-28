#!/usr/bin/env bash
INPUTDIR=$1
FILE=$2
TOOLDIR="../analysis"

FILTERV="$TOOLDIR/filter"
g++ -std=c++11 $FILTERV.cpp -o $FILTERV
UNPRED="$TOOLDIR/constFilter"
g++ -std=c++11 $TOOLDIR/constrainedFilter.cpp -o $UNPRED
DM="$TOOLDIR/dm"
g++ -std=c++11 $TOOLDIR/disasMistakes.cpp -o $DM

tar xf $INPUTDIR/$FILE.tar.xz
cat $FILE/thread* > $FILE/results
cat $FILE/results | grep H | awk '{ print $2 }' > $FILE/hiddens
cat $FILE/results | grep D | awk '{ print $2 }' > $FILE/disas

#Filter out constrained unpredictable instructions
$UNPRED $FILE/hiddens > $FILE/temp
mv $FILE/temp $FILE/hiddens

#Filter out disassembler mistakes
$DM $FILE/hiddens > $FILE/temp
mv $FILE/temp $FILE/hiddens

#Filter out ARMV8.1-A instructions
#$FILTERV $FILE/hiddens > $FILE/temp1
#mv $FILE/temp $FILE/hiddens

cp $FILE/hiddens $INPUTDIR/hiddens
rm -r $FILE
