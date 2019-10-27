# L1T Analysis Package 
# Author: George Karathanasis, georgios.karathanasis@cern.ch
# Date: 5/2019
#
import os, subprocess
from optparse import OptionParser


if __name__ == "__main__":
    """code to measure efficiency using tag & probe or orthogonal dataset"""    
    #options
    parser = OptionParser()
    parser.add_option("-i","--infile", dest="infile",  default="ntuple.txt", type=str, help="txt with ntuple list")
    parser.add_option("-p","--outpath", dest="outpath",  default="pwd", type=str, help="output path")
    parser.add_option("-o","--outfile", dest="outfile",  default="output", type=str, help="output file name")
    parser.add_option("--useOD", dest="OD",action="store_true", default=False, help="use Orthogonal Dataset method. Default tag & probe")
    parser.add_option("--emul", dest="emul",action="store_true", default=False, help="use emulator instead of data")
    parser.add_option("--probeCuts", dest="probeCuts",type=str, default="probeCuts.txt", help="probe muon cuts for TnP/muon cuts for O.D.")
    parser.add_option("--tagCuts", dest="tagCuts",type=str, default="tagCuts.txt", help="tag cuts for TnP")
    parser.add_option("--trackFinder", dest="trackFinder",type=str, default="l1t", help="use output from a tf instead ugmt, options:bmtf,omthf,emtf,l1t")

    #pass options
    (options, args) = parser.parse_args()
    print options, args

    if options.OD:
       print "Orthogonal Dataset Method"
    else:
       print "Tag&Probe Method"

    nlines=0
    with open(options.infile)  as flistfile:
         nlines=len(flistfile.readlines())
  
    if options.outpath=="pwd":
       options.outpath=os.getcwd()
       os.system("mkdir -p L1TSlimmedNtuples_Local")
       options.outpath+="/L1TSlimmedNtuples_Local";
    else:
       os.system("mkdir -p {0}".format( options.outpath))

    emulstr="false"
    if options.emul:
       emulstr="true"
	
    if not options.OD:
       os.system("root -l -q -b  EfficiencyCore/'efficiency_tree_TP.C(\"{ntuple}\",{startFile},{endFile},\"{output}\",{emul},\"{tagCuts}\",\"{probeCuts}\",\"{tf}\")'".format(
              ntuple=os.getcwd()+"/"+options.infile, startFile=0, endFile=nlines,output=options.outpath+"/"+options.outfile,emul=emulstr,tagCuts=os.getcwd()+"/"+options.tagCuts,probeCuts=os.getcwd()+"/"+options.probeCuts,tf=options.trackFinder))
    else:
       os.system("root -l -q -b  EfficiencyCore/'efficiency_tree_OD.C(\"{ntuple}\",{startFile},{endFile},\"{output}\",{emul},\"{probeCuts}\",\"{tf}\")'".format(
                ntuple=os.getcwd()+"/"+options.infile, startFile=0, endFile=nlines,output=options.outpath+"/"+options.outfile,emul=emulstr,probeCuts=os.getcwd()+"/"+options.probeCuts,tf=options.trackFinder))


