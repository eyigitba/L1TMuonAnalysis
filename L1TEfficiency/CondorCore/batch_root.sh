#!/bin/bash
# L1T Analysis Package 
# Author: George Karathanasis, georgios.karathanasis@cern.ch
# Date: 5/2019
#

NFILE=$1
shift;
NSTART=$1
echo "will start from $1 event"
shift;
NEND=$1
echo "Will end in $1 event"
shift;

OUTFILE=$1
echo "Will name it $OUTFILE";
shift;

OUTPATH=$1
echo "Will write at $OUTPATH"
shift;

EMUL=$1
echo "runining on emul $EMUL"
shift;

PROBE=$1
echo "reco mu cuts $PROBE";
shift;

TF=$1
echo "tf used $TF"
shift;

DIREC=$1
echo "Code on $DIREC directory";
shift;

TOP="$PWD"

cd ${DIREC}
cd $CMSSW_BASE
eval `scramv1 runtime -sh`
cd  ${TOP}
cp -r ${DIREC}/EfficiencyCore .
cp ${DIREC}/${NFILE} .
root -l -q -b EfficiencyCore/'efficiency_tree_OD.C("'${NFILE}'",'${NSTART}','${NEND}',"'${OUTFILE}'",'${EMUL}',"'${PROBE}'","'${TF}'")'

mkdir -p ${OUTPATH}
echo "job finished -> transfering"
cp ${OUTFILE}.root ${OUTPATH}

