#!/usr/bin/env bash
input=$1
cat $input | grep H > filteredInput.txt
python firstPass.py filteredInput.txt
cat fpOutput.txt | grep -v " 0" > fpOutputHrange.txt
cat fpOutput.txt | grep " 0" > fpOutputHsingle.txt

cat $input | grep D > filteredInput.txt
python firstPass.py filteredInput.txt
cat fpOutput.txt | grep -v " 0" > fpOutputDrange.txt
cat fpOutput.txt | grep " 0" > fpOutputDsingle.txt

rm filteredInput.txt
rm fpOutput.txt
