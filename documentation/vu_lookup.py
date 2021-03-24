#!/usr/local/bin/python3

import numpy as np 
from matplotlib import pyplot as plt 
import math

x = np.arange(1.,step=0.01) 
y = np.log10(np.sqrt(x)) + 1
plt.title("VU Meter lookup") 
plt.xlabel("x peak") 
plt.ylabel("y DB normalized") 
plt.plot(x,y)

#write array as c header file
y.tofile("vu_lookup.h", sep=",\n", format="%f")
with open("vu_lookup.h", 'r') as fin:
  data = fin.read().splitlines(True)
with open("vu_lookup.h", 'w') as fout:
  fout.writelines("#ifndef vu_lookup_h_\n")
  fout.writelines("#define vu_lookup_h_\n")
  fout.writelines("\n\nconst float vu_lookup[] PROGMEM = \n{\n")
  fout.writelines(data[1:])
  fout.writelines("\n};\n\n")
  fout.writelines("#endif\n")

plt.show()