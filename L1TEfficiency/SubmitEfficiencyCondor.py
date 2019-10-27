# L1T Analysis Package 
# Author: George Karathanasis, georgios.karathanasis@cern.ch
# Date: 5/2019
#

import os, subprocess, sys
from optparse import OptionParser
import datetime

if __name__ == "__main__":
    """ Condor implementation for efficiencies"""
   
    parser = OptionParser()
    parser.add_option("-n","--nfiles", dest="njob",   default=30,   type=int,    help="files per job")
    parser.add_option("-f","--flavor", dest="flavor", default="workday", type=str, help="condor queue (flavour) ")
    parser.add_option("-i","--infile", dest="infile",  default="ntuple.txt", type=str, help="txt with paths of input files")
    parser.add_option("-o","--outfile", dest="outfile",  default="output", type=str, help="output name")
    parser.add_option("-p","--outpath", dest="outpath",  default="pwd", type=str, help="FULL output path or leave it empty (it will the current dir)")
    parser.add_option("--useOD", dest="OD",  action="store_true",default=False, help="use orthogonal dataset method, Default tag & probe")
    parser.add_option("--emul", dest="emul",action="store_true", default=False, help="use emulator")
    parser.add_option("--probeCuts", dest="probeCuts",type=str, default="probeCuts.txt", help="probe muon cuts for TnP/muon cuts for O.D.")
    parser.add_option("--tagCuts", dest="tagCuts",type=str, default="tagCuts.txt", help="tag cuts for TnP")
    parser.add_option("--trackFinder", dest="trackFinder",type=str, default="l1t", help="use a specific tf instead of ugmt, options:bmtf,omthf,emtf,l1t")
    (options, args) = parser.parse_args()
 
    # method
    mthd="_TnP"
    if options.OD:
       print "using Orthogonal Dataset method"
       mthd="_OD"
    else:
       print "using Tag&Probe method"
   
    # data
    evt="_Data"
    emulstr="false"
    if options.emul:
       evt="_Emu"
       emulstr="true"

    print "using ",evt," events"
    print "using ",options.trackFinder," as l1 input "
    #read #of root files
    nline=0
    with open(options.infile)  as listfile:
         nline=len(listfile.readlines())

    if options.outpath=="pwd":
       options.outpath=os.getcwd()
       if not os.path.exists("L1TSlimmedNtuples"):
          os.makedirs("L1TSlimmedNtuples")
       options.outpath+="/L1TSlimmedNtuples"
   
    x= datetime.datetime.now()
    # default names
    folderName= "L1TefficiencyLog_"+str(x.year)+"_"+str(x.month)+"_"+str(x.day)+mthd+evt
    # prepare condor
    if not os.path.exists(folderName):
       os.makedirs(folderName)
       os.makedirs(folderName+"/output")
       os.makedirs(folderName+"/error")
       os.makedirs(folderName+"/log")

    # write custom log
    with open(folderName+"/efficiency_log.txt","w") as txt:
       stng="input file "+options.infile+"\n output file "+options.outfile+"\n path to save "+options.outpath+"\n method "+mthd+"\n track finder "+options.trackFinder+"\n probe cuts "+options.probeCuts+"\n tag cuts "+options.tagCuts+"\n evt "+evt
       txt.write(stng)

    # malke sensible job partition if needed     
    if nline % options.njob !=0:
       init=options.njob
       print "files ",nline," can not be segmented in ",options.njob," files/job"
       for nj in range(init,nline):
           if nline % nj ==0:
              options.njob=nj
              break;
       print "segmenting in ",options.njob," files/job "

   
    for i in xrange(0,nline,options.njob):
       strname=str(i/options.njob)
       sub= "executable = "
       if not options.OD:
          sub+="condorCore/batch_rootTP.sh\n";
          sub+="arguments = {filein} {start} {end} {fileout} {outpath} {emul} {tagCuts} {probeCuts} {tf} {direc}".format(filein=options.infile, start=i, end=i+options.njob, fileout=options.outfile+"_"+strname,outpath=options.outpath,emul=emulstr,tagCuts=options.tagCuts,probeCuts=options.probeCuts,tf=options.trackFinder,direc=" {pwd}".format(pwd=os.getcwd()))+"\n";
       else:
          sub+="CondorCore/batch_root.sh\n";
          sub+="arguments = {filein} {start} {end} {fileout} {outpath} {emul} {recoCuts} {tf} {direc}".format(filein=options.infile, start=i, end=i+options.njob, fileout=options.outfile+"_"+strname,outpath=options.outpath,emul=emulstr,recoCuts=options.probeCuts,tf=options.trackFinder,direc=" {pwd}".format(pwd=os.getcwd()))+"\n"
       sub+="output = "+folderName+"/output/job_{0}.out\n".format(strname)
       sub+="error = "+folderName+"/error/job_{0}.err\n".format(strname)
       sub+="log = "+folderName+"/log/job_{0}.log\n".format(strname)
       sub+='transfer_output_files   = ""\n'
       sub+='+JobFlavour = '+'"{flavor}"\n'.format(flavor=options.flavor)
       sub+="queue"
       with open("condor_temp.sub","w") as cdr:
          cdr.write(sub)
       cdr.close()
       subprocess.check_output(['condor_submit',"condor_temp.sub"])
       print "job "+strname+" submitted -> next job"
       subprocess.check_output(['rm','condor_temp.sub'])
    
