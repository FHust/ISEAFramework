#!/usr/bin/env python
import matplotlib
matplotlib.use('GTK')
from matplotlib.pyplot import *
import numpy as np
import sys
from save import *

from matplotlib import rcParams
rcParams['figure.figsize'] = 16, 12
rcParams.update({'figure.autolayout': True})
matplotlib.rc('xtick', labelsize=36)
matplotlib.rc('ytick', labelsize=36)
rcParams.update({'savefig.dpi': 200})
rcParams.update({'lines.markersize': 10})

y_formatter = matplotlib.ticker.ScalarFormatter(useOffset=False)
fontsize_mat = 42
folder = "pics"
name = "simulationen"
legend_mat = 36


def main(argv=sys.argv):

    # try:
    f = open(sys.argv[1], 'r')
    titles = f.readline()[1:-1].split(';')
    units = f.readline()[1:-1].split(';')
    # except:
    #    pass
    try:
        txt = np.loadtxt(sys.argv[1], delimiter=';', dtype=float)
    except IOError:
        print "Can't find file"
        exit(1)
    time = txt[:, 0]
    for i in range(1, txt.shape[1]):
        figure()
        plot(time, txt[:, i], '--o', linewidth=4)
        title(titles[i].strip(), fontsize=fontsize_mat)
        ylabel(units[i], fontsize=fontsize_mat)
        grid(True)
        xlabel("time / s", fontsize=fontsize_mat)
        ticklabel_format(style='plain')
        gca().get_xaxis().get_major_formatter().set_scientific(False)
        gca().get_yaxis().get_major_formatter().set_scientific(False)

        gca().get_xaxis().set_major_formatter(y_formatter)
        gca().get_yaxis().set_major_formatter(y_formatter)
        savePDFandSVG(folder + '/' + name + '/' + titles[i].strip())

    show()

if __name__ == "__main__":
    main(sys.argv)
    exit(0)
