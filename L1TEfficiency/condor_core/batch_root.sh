#!/bin/bash

NFILE=$1
shift;
NSTART=$1
echo "will start from $1 event"
shift;
NEND=$1
echo "Will end in $1 event"
shift;

OUTFILE=$1
echo "Will write to $OUTFILE";
shift;


set CMSSW_P='/afs/cern.ch/work/g/gkaratha/private/SUSYCMG/HLT/efficiency/Analizer/CMSSW_9_2_7/src/'
set CFG_FILE="HLTAnalysis/TriggerAnalyzer/python/ConfFile_cfg.py"
set OUTPUT_FILE="Analyzer_Output.root"
set TOP="$PWD"


echo ${NFILE},${NSTART},${NEND},"${OUTFILE}","${DIREC}"

cd "${DIREC}"
#echo $CMSSW_P

eval `scramv1 runtime -sh`
#cd $TOP
#cd /afs/cern.ch/work/g/gkaratha/private/bmtf/l1analysis2/CMSSW_10_1_4/src/L1TEfficiency
echo $PWD
#export X509_USER_PROXY=grid.pem
#voms-proxy-info -all
echo ${NFILE},${NSTART},${NEND},"${OUTFILE}"
root -l -q  'efficiency_tree_data.C(''"'${NFILE}'"'','${NSTART}','${NEND}',''"_'${OUTFILE}'"'')'
