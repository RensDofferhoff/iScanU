import numpy as np

max_cores = 20
cores_sys=16
avg_totals = np.zeros(max_cores)
for i in range(1,max_cores + 1):
	a = np.loadtxt("cores" + str(i) + "/totals_clean")
	avg_totals[i - 1] = np.average(a) / min(i,cores_sys)
avg_totals = avg_totals / 10
print(max(avg_totals))
