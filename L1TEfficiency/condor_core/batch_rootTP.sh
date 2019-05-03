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
echo "Will name it $OUTFILE";
shift;

OUTPATH=$1
echo "Will write at $OUTPATH"
shift;

DIREC=$1
echo "Code on $DIREC directory";
shift;


TOP="$PWD"

echo ${NFILE},${NSTART},${NEND},${OUTFILE},${DIREC}

cd ${DIREC}
cd $CMSSW_BASE
eval `scramv1 runtime -sh`
cd  ${TOP}
cp -r ${DIREC}/efficiency_core .
cp ${DIREC}/${NFILE} .
root -l -q -b efficiency_core/'efficiency_tree_TP.C(''"'${NFILE}'"'','${NSTART}','${NEND}',''"'${OUTFILE}'"'')'

mkdir -p ${OUTPATH}
echo "job finished -> transfering"
cp ${OUTFILE}.root ${OUTPATH}
