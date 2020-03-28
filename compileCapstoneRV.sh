#!/usr/bin/env bash

git clone https://github.com/aquynh/capstone.git
cd capstone
git checkout next
git checkout 120c80fc1454d53e1e0960b368a4797a79c03ee6
make libcapstone.a
cp -rf include/capstone/ ../disas/
cp libcapstone.a ../disas/capstone/
cd ..
rm -rf capstone
