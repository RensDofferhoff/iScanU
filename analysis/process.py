max_cores = 20
for i in range(1,max_cores + 1):
	c = 0
	inp = open("cores"+ str(i) + "/performance", "r")
	o = open("cores"+ str(i) + "/totals_clean","w")
	threads = 0
	for line in inp:
		if line.startswith('thread'):
			threads = threads + 1
		if line.startswith('=='):
			threads = 0;
		if line.startswith('Total') and line[-1] == '\n'  and threads == i:
			if c != 0:			
				o.write(line[7:-1] + '\n')	
			c = c + 1
	inp.close()
	o.close()
