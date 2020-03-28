
#!/usr/bin/env bash
OUTPUTDIR=scalingResults
OUTPUT=$OUTPUTDIR/cores
mkdir $OUTPUTDIR

PROGRAM="../bin/scanner-aarch64 "
METHOD=$2
RANGE_END=20000000
START_CORES=1
END_CORES=$1
STEP=1

CURRENT=$START_CORES
while [ $CURRENT -le $END_CORES ]; do
    rm -rf results
    $PROGRAM -$METHOD -j $CURRENT -a 4 -e $RANGE_END
    mkdir $OUTPUT$CURRENT
    cp results/performance $OUTPUT$CURRENT
    cat results/performance | grep Total | awk '{ print $2/10 }' > temp
    head -n -1 temp > $OUTPUT$CURRENT/totals
    let "CURRENT=CURRENT+1"
done
rm temp
