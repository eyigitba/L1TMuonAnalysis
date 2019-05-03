import os, subprocess, sys
import argparse

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-i","--input_histo", dest="infile",   default="plots.txt",   type="string",    help="txt with histo")
    parser.add_option("-s","--script", dest="script", default="EfficiencyPlots", type="string", help="Options: EfficiencyPlots or SimplePlots")
    parser.add_option("-n","--ntuple" ,dest="inntuple",  default="pwd", type="string", help="give the FULL ntuple path. by default checks for L1TSlimmedNtuples")
    parser.add_option("--plot_folder",dest="plot_folder",  default="none", type="string", help="give the preferred directory name to save plots. none(default) means no automatic saving")
(options, args) = parser.parse_args()
#print options, args
if not os.path.exists(options.infile): 
   print "please, give a valid txt" 
   exit() 

if options.script !="EfficiencyPlots" and options.script !="SimplePlots":
   print "invallid plotting option. Select EfficiencyPlots or SimplePlots"
   exit()
if options.inntuple=="pwd":
   options.inntuple=os.getcwd()+"/L1TSlimmedNtuples"
if not os.path.exists(options.inntuple):
   print "please, give a valid path with ntuples"
   exit()
if options.plot_folder=="none":
   print "plots will NOT be saved"
else:
   print "plots will be saved at "+options.plot_folder
   if not os.path.exists(options.plot_folder):    
      os.makedirs(options.plot_folder)
script=""
if  options.script =="EfficiencyPlots":
	script="plotEfficiency.C"
else:
	script="plotSimple.C"
os.system("root -l plotting_core/'{scr}(\"{filein}\",\"{ntuple}\",\"{save}\")'".format( scr=script, filein=options.infile, ntuple=options.inntuple , save=os.getcwd()+"/"+options.plot_folder) )
