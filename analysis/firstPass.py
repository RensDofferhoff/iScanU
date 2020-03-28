import numpy as np
import sys
import math

def read_integers(filename):
    with open(filename) as f:
        return map(int, f)

def isPower2(num):
	return num != 0 and ((num & (num - 1)) == 0)

print("starting\n")
inputData = read_integers(sys.argv[1]);
print("data loaded\n")
f = open("fpOutput.txt", "w+")

#NOTE FIRST PASS ---------------------------------------------------------------
i = 0
j = 0
delta = inputData[1] - inputData[0]
while i < len(inputData):
	begin = i
	if isPower2(delta):
		while i < len(inputData) - 1 and inputData[i] + delta == inputData[i + 1]:
			i += 1
	end = i
	diff = end - begin
	variableBits = diff.bit_length()
	if not isPower2(diff + 1):
		variableBits -= 1
		i -= int(diff) + 1 - int(2**variableBits)
	deltaBitShift = delta.bit_length() - 1
	mask = 0
	for k in range(variableBits):
		mask |= 1 << k + deltaBitShift
	s = str(inputData[begin]) + ' ' + str(mask) + '\n'
	f.write(s)

	j += 1
	i += 1
	if i + 1 < len(inputData):
		delta = inputData[i + 1] - inputData[i]
f.close()
