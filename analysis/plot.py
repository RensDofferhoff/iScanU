import matplotlib.pyplot as plt
import numpy as np

max_cores = 20
cores_sys=16
avg_totals = np.zeros(max_cores)
error = np.zeros(max_cores)
for i in range(1,max_cores + 1):
	a = np.loadtxt("cores" + str(i) + "/totals_clean")
	avg_totals[i - 1] = np.average(a)
	error[i - 1] = np.std(a)
avg_totals = avg_totals / 10
perfect=np.arange(1,cores_sys+1) * avg_totals[0]
print(np.max(avg_totals))
ax = plt.gca()
ax.tick_params(axis = 'both', which = 'major', labelsize = 11)
ax.tick_params(axis = 'both', which = 'minor', labelsize = 11)
plt.ylabel("IPS", fontsize=13)
plt.xlabel("Numbers of scanner units", fontsize=13)
plt.title("Scaling ptrace method on 16-core Graviton affinity set", fontsize=15)
plt.xticks(range(1,23,3))
plt.errorbar(np.arange(1, max_cores + 1), avg_totals, yerr=error, fmt='-b.', label='scan results')
plt.plot(np.arange(1, cores_sys + 1), perfect, label='perfect scaling')
plt.legend(loc='upper left',fontsize = 'large')

plt.tight_layout()
plt.savefig("scaling16pt-A.pdf")


