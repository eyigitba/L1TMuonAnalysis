# Package for L1T muon Analysis

This code provides algorithms for analysis on L1 muons. Currently implemented:
  - Efficiency with two methods ( Tag & Probe, Orthogonal dataset )
  - Simple plots for rate and in-depth analysis
  
  
  
# Instalation
In a CMSSW folder copy this package:
  - cd ${CMSSW_BASE}/src
  - git clone https://github.com/gkaratha/L1TMuonAnalysis


# Efficiency
Takes as input L1T ntuples that have data and/or emulator result. The full path of the ntuples should be written to a txt and provited to the code. To easilly create this txt use NtupleListCreator.py:
  - python NtupleListCreator.py -i path of ntuples -o name of output txt -m max number of ntuples to be written
  
To run efficiency locally use the RunEfficiency.py. Usage for Tag & Probe method (T&P) and flags explanation:
  - python RunEfficiency.py -i txt with ntuple paths -o path to save slimmed ntuples --probeCuts txt with probe muon cuts --tagCuts tag muon cuts
  - Flags:
    - useOD: uses Orthogonal dataset method instead of T&P. In that case tagCuts option is redudant
    - emul: run on emulator branches
    - trackFinder: uses the output of a specific track finder instead of uGMT. Options: bmtf omtf emtf
  
  - To run on condor:
    - use SubmitEfficiencyCondor.py. The same options that are implemented for the local run, are also implemented for condor run. In addition there are two more flags:
      - n: number of files per condor 
      - f: condor flavour 
    - example: python ubmitEfficiencyCondor.py -n number of files / job -f flavour -i txt with ntuple paths -o path to save slimmed ntuples --probeCuts txt with probe muon cuts --tagCuts tag muon cuts


