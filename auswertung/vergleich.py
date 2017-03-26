#!/usr/bin/env python
import matplotlib
matplotlib.use('GTK')
from matplotlib.pyplot import *
import numpy as np
import sys
from scipy.interpolate import interp1d


def main(argv=sys.argv):
    try:
        txt1 = np.loadtxt(sys.argv[1], delimiter=';', dtype=float)
    except IOError:
        print "Can't find file"
        exit(1)

    try:
        txt2 = np.loadtxt(sys.argv[2], delimiter='\t', dtype=float, skiprows=1)
    except IOError:
        print "Can't find file"
        exit(1)

    f1 = interp1d(txt1[:, 0], txt1[:, 2], kind='cubic')
    f2 = interp1d(txt2[:, 0], txt2[:, 1], kind='cubic')

    T = np.minimum(np.max(txt1[:, 0]), np.max(txt2[:, 0]))

    t = np.linspace(0, T, 20)
    y1 = f1(t)
    y2 = f2(t)

    # Beide Datensaetze
    f, axarr = subplots(2, sharex=True)
    axarr[0].plot(t, y1, 'ro--', label="Framework")
    axarr[0].plot(t, y2, 'bv--', label="LTSpice")
    axarr[0].grid(True)

    axarr[0].set_title("Vergleich LTSpice mit Framework")
    axarr[0].set_xlabel("Zeit [s]")
    axarr[0].set_ylabel("Spannung [V]")
    axarr[0].legend()

    # Differenz
    axarr[1].plot(t, y2 - y1, 'ro--', label="LTSpice - Framework")
    axarr[1].grid(True)

    axarr[1].set_title("Vergleich LTSpice mit Framework, Differenz")
    axarr[1].set_xlabel("Zeit [s]")
    axarr[1].set_ylabel("Spannung [V]")
    # axarr[1].legend()
    show()


if __name__ == "__main__":
    main(sys.argv)
    exit(0)
