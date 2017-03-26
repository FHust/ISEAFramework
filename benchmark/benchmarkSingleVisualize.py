#!/usr/bin/env python
import matplotlib
matplotlib.use('GTK')
from matplotlib.pyplot import *
import numpy as np
import sys

def main(argv = sys.argv):
    try:
	txt=np.genfromtxt(sys.argv[1], dtype=[('mean','f8'),('runs','i8'),('elements','i8')], delimiter=';')
    except IOError:
        print "Can't find file"
        exit(1)

    #figure('Benchmark mit {zellen}  Zellen'.format(zellen=cellelements))
    plot(txt['elements'], txt['mean'])
    show()

if __name__ == "__main__":
    main(sys.argv)
    exit(0)
