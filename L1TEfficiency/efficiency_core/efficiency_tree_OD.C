#include <iostream>
#include "TH1F.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TChain.h"
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <vector>
#include "TString.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"
#include "analysis.h"
#include "CutReader.h"

int efficiency_tree_OD(TString file,int start,int end, TString name,bool emul=false,TString Cuts="Cuts.txt",TString TF="l1mu"){

TString L1tree= "l1UpgradeTree/L1UpgradeTree";
if (emul) L1tree= "l1UpgradeEmuTree/L1UpgradeTree";
TString TFtree="l1UpgradeTfMuonTree/L1UpgradeTfMuonTree";
if (emul) TFtree="l1UpgradeTfMuonEmuTree/L1UpgradeTfMuonTree";


TChain * cc_tf=new TChain(TFtree); TChain*cc_muon=new TChain(L1tree);
TChain*cc_reco=new TChain("l1MuonRecoTree/Muon2RecoTree");

std::ifstream file2(file); std::string str; std::string file_contents;
int line=-1;
while (std::getline(file2, str)){
  line++;
  if (line<start || line>end || line==end) continue;
  TString str2(str); cout<<str2<<endl;
  cc_tf->Add(str2); cc_muon->Add(str2); cc_reco->Add(str2);
}

TFile *fout =new TFile(name+".root","RECREATE");
TTree * t1 =new TTree("efftree","efftree");

CutReader recoCuts(Cuts);

std::vector<float> pt_probe,eta_probe,phi_probe,DRprobel1,isoprobe,pt_l1probe,eta_l1probe,phi_l1probe,DRtagprobe,qual_l1probe;

t1->Branch("pt_probe",& pt_probe); t1->Branch("eta_probe",& eta_probe);
 t1->Branch("phi_probe",& phi_probe); t1->Branch("iso_probe",& isoprobe);
 t1->Branch("pt_l1probe",& pt_l1probe); t1->Branch("eta_l1probe",& eta_l1probe);
 t1->Branch("phi_l1probe",&phi_l1probe); t1->Branch("qual_l1probe",&qual_l1probe);
 t1->Branch("DR_l1probe",& DRprobel1);


TTreeReader tfReader(cc_tf);
TTreeReaderValue<L1Analysis::L1AnalysisL1UpgradeTfMuonDataFormat> bmtf(tfReader,"L1UpgradeBmtfMuon");
TTreeReaderValue<L1Analysis::L1AnalysisL1UpgradeTfMuonDataFormat> omtf(tfReader,"L1UpgradeOmtfMuon");
TTreeReaderValue<L1Analysis::L1AnalysisL1UpgradeTfMuonDataFormat> emtf(tfReader,"L1UpgradeEmtfMuon");

TTreeReader recoReader(cc_reco);
TTreeReaderValue<unsigned short> numreco(recoReader,"nMuons");
TTreeReaderArray<float> ptreco(recoReader,"pt");
TTreeReaderArray<float> etareco(recoReader,"etaSt2");
TTreeReaderArray<float> phireco(recoReader,"phiSt2");
TTreeReaderArray<bool> tightreco(recoReader,"isTightMuon");
TTreeReaderArray<float> isoreco(recoReader,"iso");
TTreeReader gmtReader(cc_muon);
TTreeReaderValue<unsigned short> l1muons(gmtReader,"nMuons");
TTreeReaderArray<float> l1mupt(gmtReader,"muonEt");
TTreeReaderArray<float> l1mueta(gmtReader,"muonEta");
TTreeReaderArray<float> l1muphi(gmtReader,"muonPhi");
TTreeReaderArray<unsigned short> l1muqual(gmtReader,"muonQual");
TTreeReaderArray<short> l1mubx(gmtReader,"muonBx");


while(recoReader.Next() && gmtReader.Next()){
 if (TF=="bmtf" || TF=="omtf" || TF=="emtf")
   tfReader.Next();
 for (int j=0; j<*numreco; j++){
  if(fabs(etareco[j])>recoCuts.maxEta()) continue;
  if(fabs(etareco[j])<recoCuts.minEta()) continue;
  if(fabs(ptreco[j])<recoCuts.minPt()) continue;
  if(tightreco[j]==0) continue;
  if (isoreco[j]>=recoCuts.hltIso()) continue;
  isoprobe.push_back(isoreco[j]); pt_probe.push_back(ptreco[j]);
  eta_probe.push_back(etareco[j]); phi_probe.push_back(phireco[j]);
  std::vector<std::pair<struct l1muon,float>>  mupairs;
  if (TF=="bmtf")
    mupairs= RecoTF_Pairs(bmtf->tfMuonBx,bmtf->tfMuonHwQual,etareco[j],phireco[j],bmtf->tfMuonHwPt,bmtf->tfMuonHwEta,bmtf->tfMuonGlobalPhi);
  else if (TF=="omtf")
    mupairs= RecoTF_Pairs(omtf->tfMuonBx,omtf->tfMuonHwQual,etareco[j],phireco[j],omtf->tfMuonHwPt,omtf->tfMuonHwEta,omtf->tfMuonGlobalPhi);
  else if (TF=="emtf")
    mupairs= RecoTF_Pairs(emtf->tfMuonBx,emtf->tfMuonHwQual,etareco[j],phireco[j],emtf->tfMuonHwPt,emtf->tfMuonHwEta,emtf->tfMuonGlobalPhi);
  else
    mupairs= RecoL1_Pairs(-1,(*(&l1mubx)),(*(&l1muqual)),etareco[j],phireco[j],(*(&l1mupt)),(*(&l1mueta)),(*(&l1muphi)));
  
 if(mupairs.size()>0){
   std::sort(mupairs.begin(),mupairs.end(),sort_vpair_lowHigh);
   if (mupairs[0].second>recoCuts.drL1RecoCone()) continue;
   DRprobel1.push_back(mupairs[0].second);  
   pt_l1probe.push_back(mupairs[0].first.pt);
   eta_l1probe.push_back(mupairs[0].first.eta);
   phi_l1probe.push_back(mupairs[0].first.phi);
   qual_l1probe.push_back(mupairs[0].first.qual);
  }

 }

t1->Fill();
}



t1->Write();

return 0;
}
