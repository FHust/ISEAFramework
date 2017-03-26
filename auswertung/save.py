#!/usr/bin/env python
# -*- coding: latin-1 -*-

import numpy as np
import os
from matplotlib.pyplot import *


def savePDFandSVG(name, length=False):
    suffix = ""
    iteration = 0
    if length:
        gcf().gca().legend(loc='upper left', bbox_to_anchor=(1, 1.02))
        ax = gcf().gca()
        box = ax.get_position()
        ax.set_position(
            [box.x0 + 0.02, box.y0, box.width * length, box.height])
    dirname = os.path.dirname(name)
    if not os.path.exists(dirname):
            os.makedirs(dirname)
    while True:
        try:
            with open(name + suffix + ".png"):
                suffix = str(iteration)
                iteration += 1
                continue

        except IOError:

            break

#    savefig(name+".pdf", quality = 100 , dpi = 150)
#    savefig(name+suffix+".svg", dpi = 150)
    savefig(name + suffix + ".png", dpi=150)
