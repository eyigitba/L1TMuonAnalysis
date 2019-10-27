import os, subprocess 
from optparse import OptionParser



if __name__ == "__main__":

    # availiable plotting options
    parser = OptionParser()
#    parser.add_option("--help", action="help")
    parser.add_option("-p","--plotsTxtFile", dest="infile",   default="plots.txt", type="string", help="txt with selected plots")
    parser.add_option("-t","--type", dest="script", default="EfficiencyPlots", type="choice",choices= ['EfficiencyPlots','SimplePlots'], help="plotter type choices: EfficiencyPlots or SimplePlots")
    parser.add_option("-i","--inputData" ,dest="inntuple",  default="pwd", type="string", help="give the FULL ntuple path. by default checks for L1TSlimmedNtuples")
    parser.add_option("-o","--outputDir",dest="plot_folder",  default="L1Tplots", type="string", help="folder name to save plots. Default=L1Tplots. If 'none' selected, plots not saved")
    parser.add_option("--ntupleDataEff",dest="inntupleData",  default="none", type="string", help="path for data IF we want data/emu superimposed")
    parser.add_option("--ntupleEmulEff",dest="inntupleEmul",  default="none", type="string", help="path for emulator IF we want data/emu superimposed")
(options, args) = parser.parse_args()

# sanity checks
if not os.path.exists(options.infile): 
   print "please, give a valid txt" 
   exit() 
if options.inntupleData!="none" and options.inntupleEmul!="none":
   print "going to superimpose data and emulator efficiency plots"
   options.inntuple=options.inntupleData
   if options.script !="EfficiencyPlots":
      print "Script is selected SimplePlots but data emul files are given. If you want to plot Simple plots, remove ntupleDataEff and ntupleEmulEff and give ntuple option. If you want efficiency plots; rerun with EfficiencyPlots as script option"
      exit()
   
if options.inntuple=="pwd":
   options.inntuple=os.getcwd()+"/L1TSlimmedNtuples"

if not os.path.exists(options.inntuple):
   print "please, give a valid path with ntuples, or 2 directories for emulator vs data efficiency"
   exit()

#saving plots or not
if options.plot_folder=="none":
   os.system("mkdir -p L1Tplots")
   print "plots will be saved at L1Tplots"
   options.plot_folder="L1Tplots"
else:
   print "plots will be saved at "+options.plot_folder
   if not os.path.exists(options.plot_folder):    
      os.makedirs(options.plot_folder)

script=""
if options.script =="EfficiencyPlots":
   script="plotEfficiency.C"
else:
   script="plotSimple.C"

if options.inntupleData!="none" and options.inntupleEmul!="none":    
   os.system("root -l -q -b PlottingCore/'{scr}(\"{filein}\",\"{ntuple}\",\"{save}\",\"{ntuple2}\")'".format( scr=script, filein=options.infile, ntuple=options.inntuple , save=os.getcwd()+"/"+options.plot_folder,ntuple2=options.inntupleEmul) )
else:
   os.system("root -l -q -b PlottingCore/'{scr}(\"{filein}\",\"{ntuple}\",\"{save}\")'".format( scr=script, filein=options.infile, ntuple=options.inntuple , save=os.getcwd()+"/"+options.plot_folder) )
