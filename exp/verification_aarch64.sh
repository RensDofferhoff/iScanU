#!/usr/bin/env bash
rm -rf results
../injector -m -b 0 -e 16
../injector -m -b 3560964060 -e 3560964070
../injector -m -b 3569352660 -e 3569352680
cp results/thread0 verificationMC
rm -rf results

../injector -p -b 0 -e 16
../injector -p -b 3560964060 -e 3560964070
../injector -p -b 3569352660 -e 3569352680
cp results/thread0 verificationPT
rm -rf results

cat verificationMC | grep H > temp
mv temp verificationMC
cat verificationPT | grep H > temp
mv temp verificationPT
