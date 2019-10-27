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


# Plotting
After the efficiency step proccessed slimmed ntuples will be create it. In the plotting step we use them as inputs. The main module for plots is the "plotter.py". Usage:
  - python plotter.py -p plots.txt -i <path/eith/slim-tuples> -o <outputDir>
  - Flags:
    - t: discriminate between simple plotter (for pure L1T histograms and rates) and efficiency plotter
    - ntupleDataEff: if we want to superimpose data/emulator instead of using "i" we should give ntuples that have data response and emulator separetaly. This flag sets the data input
    - ntupleEmulEff: this sets the emulator (or MC). Every plot in plots.txt will be produced for data/emulator and then superimposed
  
  - plots.txt:
    - Includes every plot we wat to produce along with cuts and several customization options
    - Example for efficiency plotter: title:variable:denominator cuts:numerator EXTRA cuts:labelX:bins:start:end:canvas name
    - Example for simple plotter: title: variable : cuts : data/emu :x axis name:bins:x axis begin : x axis end: canvas
    - Consecutive plots in the same canvas are superimposed automatically. If no legend name is provided histo name will be used. Colors will change automatically as well
    - Optional arguments can be set at the end of the line (after canvas option):
      - YTitle: sets Y axis title
      - LineColor: sets line color (according to ROOT pallete [int])
      - XLabelSize: sets size of X axis title
      - YLabelSize: same for Y axis
      - Fit: fits the plot. For fit we can use: Gauss Erf Exp and Pol2. Also the range is mandatory (eg Fit=Erf,0,50)
      - Norm: normalize histogram to 1 (not used in efficiency plots)
      - Rate: computes rate in Hz (usage: Rate= numberof bunches, number of events)
