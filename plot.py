import os
import re
import numpy as np
import matplotlib.pyplot as plt

freq = 350e+6
dir = "./Microbenchmarks/Arithmetic-Throughput/profile"
p = re.compile(r'([A-Z]+)_([A-Z0-9]+)_tl([0-9]+)')
data = dict()
plt.rcParams['figure.constrained_layout.use'] = True
for root, dirs, files in os.walk(dir):
    for filename in files:
        m = p.match(filename)
        op = m.groups()[0]
        tp = m.groups()[1]
        tasklets = int(m.groups()[2])
        if tp[0] == 'U':
            continue
        if tp not in data.keys():
            data[tp] = dict()
        if op not in data[tp].keys():
            data[tp][op] = np.empty(25, dtype=float)
        f = open(os.path.join(dir, filename))
        raw_data = f.read()
        nr_elements = int(re.search(r'nr_elements\s([0-9]*)', raw_data).groups()[0])
        cycles = float(re.search(r'DPU cycles\s*=\s*([0-9\.e\+]*)', raw_data).groups()[0])
        mops = nr_elements * freq / cycles / 1e6
        data[tp][op][tasklets] = mops

x_data = np.arange(1, 25, dtype=int)
xi = list(range(1, len(x_data), 2))
yi = list(range(0, 50, 10))

for i, tp in enumerate(['INT32', 'INT64', 'FLOAT', 'DOUBLE']):
    plt.subplot(2,2,i+1)
    plt.title('{} (1 DPU)'.format(tp), fontsize = 'x-small')
    plt.ylabel('Arithmetic Throughput (MOPS)', fontsize = 'x-small')
    plt.xlabel('#Tasklets', fontsize = 'x-small')
    plt.grid(linestyle='--', alpha=0.5)
    plt.xticks(xi)
    plt.plot(x_data, data[tp]['ADD'][1:], 'ro-', label='ADD')
    plt.plot(x_data, data[tp]['SUB'][1:], 'y^-', label='SUB')
    plt.plot(x_data, data[tp]['MUL'][1:], 'bs-', label='MUL')
    plt.plot(x_data, data[tp]['DIV'][1:], 'gx-', label='DIV')
    plt.legend(loc = (0.7, 0.45), fontsize = 'x-small')
plt.savefig("interleaved_throughput.pdf")

