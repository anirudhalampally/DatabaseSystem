import random
import sys
import os
import subprocess
import matplotlib.pyplot as plt

num = 1
x=[]
y=[]
for i in range(10):
	x.append(num)
	miss =0
	outfile = "./plot_page_num/out_files_5/out_page_num_" + str(num)
	outfile = open(outfile,"r")
	for line in outfile:
	  if "MISS" in line:
	    miss+=1;
	outfile.close()
	num+=1
	y.append(miss)
print(y)
plt.plot(x,y)
plt.xlabel("Number Of Pages")
plt.ylabel("Number Of Misses")
plt.title('Number Of Pages vs Number Of Misses')
plt.grid(True)
plt.text(1500,80,"Page Size = 300 Bytes")
plt.show()
