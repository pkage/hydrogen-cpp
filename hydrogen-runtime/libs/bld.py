#! /usr/bin/env python

import os
import re


for f in os.listdir("."):
    if (f[-4:] == ".cpp"):
        print "compiling "  + f
        os.system("clang++ -dynamiclib " + f + " -o out/" + f[:-3] + "hylib");
