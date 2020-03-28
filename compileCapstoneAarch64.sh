#!/usr/bin/env bash
cd disas/capstone/capstone-4.0.1/
make libcapstone.a
cp -rf include/capstone/ ../
cp libcapstone.a ../
