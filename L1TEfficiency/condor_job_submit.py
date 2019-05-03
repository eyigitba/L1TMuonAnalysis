import os, subprocess, sys
import argparse
import datetime

if __name__ == "__main__":
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option("-n","--nfiles", dest="njob",   default=30,   type=int,    help="files per job")
    parser.add_option("-f","--flavor", dest="flavor", default="workday", type=str, help="queue")
    parser.add_option("-i","--inputfile", dest="infile",  default="ntuple.txt", type=str, help="add txt with input file paths")
    parser.add_option("-o","--outputfile", dest="outfile",  default="output", type=str, help="add the output name")
    parser.add_option("-p","--outputpath", dest="outpath",  default="pwd", type=str, help="add the FULL output path or leave it empty")
    parser.add_option("-t","--useTnP", dest="tAp",  action="store_true",default=True, help="use tag and probe")
(options, args) = parser.parse_args()
print options, args

if options.tAp:
  print "using Tag&Probe method"
else:
  print "using orthogonal dataset method"

nline=0
with open(options.infile)  as listfile:
  nline=len(listfile.readlines())

if options.outpath=="pwd":
   options.outpath=os.getcwd()
   if not os.path.exists("L1TSlimmedNtuples"):
      os.makedirs("L1TSlimmedNtuples")
   options.outpath+="/L1TSlimmedNtuples"
   
x= datetime.datetime.now()
folderName= "L1TefficiencyLog_"+str(x.year)+"_"+str(x.month)+"_"+str(x.day)
if not os.path.exists(folderName):
    os.makedirs(folderName)
    os.makedirs(folderName+"/output")
    os.makedirs(folderName+"/error")
    os.makedirs(folderName+"/log")
    

if nline % options.njob !=0:
   print "files ",nline," can not be segmented in ",options.njob," files/job"
   for nj in range(nline-1,1,-1):
     if nline % nj ==0:
        options.njob=nj
        break;

   print "segmenting in ",options.njob," files/job "
#icount   
for i in xrange(0,nline,options.njob):
  strname=str(i/options.njob)
  with open("condor_temp.sub","w") as cdr:
    sub= "executable = "
    if options.tAp:
       sub+="condor_core/batch_rootTP.sh\n";
    else:
       sub+="condor_core/batch_root.sh\n";
    sub+="arguments = {filein} {start} {end} {fileout} {outpath} {direc}".format(filein=options.infile, start=i, end=i+options.njob, fileout=options.outfile+"_"+strname,outpath=options.outpath,direc=" {pwd}".format(pwd=os.getcwd()))+"\n"
    sub+="output = "+folderName+"/output/job_{0}.out\n".format(strname)
    sub+="error = "+folderName+"/error/job_{0}.err\n".format(strname)
    sub+="log = "+folderName+"/log/job_{0}.log\n".format(strname)
    sub+='transfer_output_files   = ""\n'
    sub+='+JobFlavour = '+'"{flavor}"\n'.format(flavor=options.flavor)
    sub+="queue"
    cdr.write(sub)
  cdr.close()
  subprocess.check_output(['condor_submit',"condor_temp.sub"])
  print "job "+strname+" submitted -> next job"
  subprocess.check_output(['rm','condor_temp.sub'])
  
    
 
