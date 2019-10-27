# L1T Analysis Package 
# Author: George Karathanasis, georgios.karathanasis@cern.ch
# Date: 5/2019
#

import fileinput
import os.path, subprocess, sys
import re

if __name__ == "__main__":
 """code to create the txt file that contains path of all ntuples. input full path output name.txt"""
 from optparse import OptionParser
 parser = OptionParser()
 parser.add_option("-i","--init_path", dest="path",   default="",   type="string",    help=" full path of files ")
 parser.add_option("-o","--output", dest="name", default="ntuple", type="string", help="output txt name")
 parser.add_option("-m","--maxfiles", dest="maxfiles", default=-1, type=int, help="max number of files to run on")
 (options, args) = parser.parse_args()
 outlines=""; count=0;
 if "afs" not in options.path and "eos" not in options.path:
   print "The path does not start from afs or eos. Is it correct?"
 for dirpath, dirnames, filenames in os.walk(options.path):
  for filename in [f for f in filenames if f.endswith(".root")]:
     outlines+=os.path.join(dirpath,filename)+"\n"
     if options.maxfiles!=-1 and count>options.maxfiles:
        break
     count+=1
 if outlines=="":
   print "no files found. exiting"
   exit()
 with open(options.name+".txt", 'w') as fl:
    fl.write(outlines)
 fl.close()

