#!/usr/bin/env python
import matplotlib
matplotlib.use('GTK')
from matplotlib.pyplot import *
import numpy as np
import sys

def main(argv = sys.argv):
    try:
	txt=np.genfromtxt(sys.argv[1], dtype=[('worker','i8'),('run','i8'),('cells','i8'),('workload','S5'),('mean','f8'),('min','f8')], delimiter=';')
    except IOError:
        print "Can't find file"
        exit(1)
    
    workercount = np.amax( txt['worker'] )
    cellelements = txt['cells'][0]
    runs = np.amax( txt['run'] )
    
    #worker,meantime
    i = 0
    graphdata = np.zeros(shape=( workercount, 1 ))
    for worker in range(0, workercount):
	meantime=0
	for run in range(0, runs):
	    meantime+=txt['mean'][i]
	    i=i+1
	graphdata[worker] = meantime / runs	
            
    figure('Benchmark mit {zellen}  Zellen'.format(zellen=cellelements))
    x=range(1, workercount +1)
    plot(x, graphdata)
    xticks( x )
    show()

if __name__ == "__main__":
    main(sys.argv)
    exit(0)
