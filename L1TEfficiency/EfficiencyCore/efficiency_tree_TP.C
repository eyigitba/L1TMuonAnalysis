/* L1T analysis package 
 * module: main module of Tag & Probe method 
 * Author: George Karathanasis, georgios.karathanasis@cern.ch
 *
 *
 */

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


int efficiency_tree_TP(TString file="skata",int start=0,int end=1000, TString name="skata",bool emul=false,TString TagCuts="Cuts.txt",TString ProbeCuts="Cuts.txt",TString TF="l1mu"){
 
  // read data/mc from ugmt
  TString L1tree= "l1UpgradeTree/L1UpgradeTree";
  if (emul) L1tree= "l1UpgradeEmuTree/L1UpgradeTree";

  // read also tf outputs
  TString TFtree="l1UpgradeTfMuonTree/L1UpgradeTfMuonTree";
  if (emul) TFtree="l1UpgradeTfMuonEmuTree/L1UpgradeTfMuonTree";
  
  TChain*cc_l1=new TChain(L1tree);
  TChain*cc_reco=new TChain("l1MuonRecoTree/Muon2RecoTree");
  TChain*cc_tf=new TChain(TFtree);

  std::ifstream file2(file);
  std::string str;
  std::string file_contents;
  int line=-1;
  while (std::getline(file2, str)){
    line++;
    if (line<start || line>end || line==end) continue;
    TString str2(str);
    cout<<"read file "<<str2<<endl;
    cc_l1->Add(str2);
    cc_reco->Add(str2);
    cc_tf->Add(str2);
  }


  TFile *fout =new TFile(name+".root","RECREATE");
  TTree * t1 =new TTree("efftree","efftree");

  //reading cuts
  CutReader tagCuts(TagCuts);
  CutReader probeCuts(ProbeCuts);

  float pt_tag, eta_tag, phi_tag, minDRtagl1, isotag, pt_l1tag, eta_l1tag,
        phi_l1tag, DRtagl1;

  std::vector<float> pt_probe, eta_probe, phi_probe, DRprobel1, isoprobe,
                     pt_l1probe, eta_l1probe, phi_l1probe, DRtagprobe,
                     qual_l1probe;

  t1->Branch("pt_tag",& pt_tag);
  t1->Branch("eta_tag",& eta_tag);
  t1->Branch("phi_tag",& phi_tag);
  t1->Branch("iso_tag",& isotag);
  t1->Branch("pt_l1tag",& pt_l1tag);
  t1->Branch("eta_l1tag",& eta_l1tag);
  t1->Branch("phi_l1tag",& phi_l1tag);
  t1->Branch("DR_l1tag",& DRtagl1);

  t1->Branch("pt_probe",& pt_probe);
  t1->Branch("eta_probe",& eta_probe);
  t1->Branch("phi_probe",& phi_probe);
  t1->Branch("iso_probe",& isoprobe);
  t1->Branch("pt_l1probe",& pt_l1probe); 
  t1->Branch("eta_l1probe",& eta_l1probe);
  t1->Branch("phi_l1probe",&phi_l1probe); 
  t1->Branch("qual_l1probe",&qual_l1probe);
  t1->Branch("DR_l1probe",& DRprobel1); 
  t1->Branch("DR_tag_probe",& DRtagprobe);

  TTreeReader tfReader(cc_tf);
  TTreeReaderValue<L1Analysis::L1AnalysisL1UpgradeTfMuonDataFormat> bmtf(tfReader,"L1UpgradeBmtfMuon");
  TTreeReaderValue<L1Analysis::L1AnalysisL1UpgradeTfMuonDataFormat> omtf(tfReader,"L1UpgradeOmtfMuon");
  TTreeReaderValue<L1Analysis::L1AnalysisL1UpgradeTfMuonDataFormat> emtf(tfReader,"L1UpgradeEmtfMuon");

  TTreeReader gmtReader(cc_l1);
  TTreeReaderValue<unsigned short> l1muons(gmtReader,"nMuons");
  TTreeReaderArray<float> l1mupt(gmtReader,"muonEt");
  TTreeReaderArray<float> l1mueta(gmtReader,"muonEta");
  TTreeReaderArray<float> l1muphi(gmtReader,"muonPhi");
  TTreeReaderArray<unsigned short> l1muqual(gmtReader,"muonQual");
  TTreeReaderArray<short> l1mubx(gmtReader,"muonBx");

  TTreeReader recoReader(cc_reco);
  TTreeReaderValue<unsigned short> numreco(recoReader,"nMuons");
  TTreeReaderArray<float> ptreco(recoReader,"pt");
  TTreeReaderArray<float> etareco(recoReader,"eta");
  TTreeReaderArray<float> phireco(recoReader,"phi");
  TTreeReaderArray<int> chargereco(recoReader,"charge");
  TTreeReaderArray<bool> tightreco(recoReader,"isTightMuon");
  TTreeReaderArray<float> isoreco(recoReader,"iso");
  TTreeReaderArray<short> hlt_mu(recoReader,"hlt_isomu");
  TTreeReaderArray<float> hlt_DR(recoReader,"hlt_isoDeltaR");


  cout<<"total evts to process "<<cc_reco->GetEntries()<<endl;

  int evt=0;
  while(recoReader.Next() && gmtReader.Next()){

    if (evt%1000==0) cout<<evt<<endl;

    // if we want the output of a specific TF only read evt
    if (TF=="bmtf" || TF=="omtf" || TF=="emtf") 
       tfReader.Next();

    evt++;
    if (*numreco<2) continue; // cannot have tag & probe without 2 mu

    // initialize branches
    pt_tag=-9, eta_tag=-9, phi_tag=-9, minDRtagl1=-9, isotag=-9,
    pt_l1tag=-9, eta_l1tag=-9, phi_l1tag=-9, DRtagl1=-9;
    pt_probe.clear(); eta_probe.clear(); phi_probe.clear(); DRprobel1.clear();
    isoprobe.clear(); pt_l1probe.clear(); eta_l1probe.clear();
    phi_l1probe.clear(); DRtagprobe.clear(); qual_l1probe.clear();

    //find good tags
    std::vector<unsigned int> tagindex, probeindex;

    for (unsigned int j=0; j<*numreco; j++){

      if(fabs(etareco[j])>tagCuts.maxEta()) continue;
      if(fabs(etareco[j])<tagCuts.minEta()) continue;
      if(!tightreco[j]) continue; 
      if (isoreco[j]>=tagCuts.hltIso()) continue;
      if (hlt_mu[j]!=1) continue;
      if (hlt_DR[j]>=tagCuts.maxHLTDrCone()) continue;
      if (ptreco[j]<tagCuts.minPt()) continue;
      tagindex.push_back(j);
    }

    if (tagindex.size()==0) continue;

    // find good probes
    for (unsigned int j=0; j<*numreco; j++){
      if(fabs(etareco[j])>probeCuts.maxEta()) continue;
      if(fabs(etareco[j])<probeCuts.minEta()) continue;
      if(!tightreco[j]) continue; 
      if (isoreco[j]>=probeCuts.hltIso()) continue;
      probeindex.push_back(j);
    }

    if (probeindex.size()==0) continue;

    // match tag mu to l1 mu
    std::vector<std::pair<unsigned int,unsigned int>> l1tagpair;
    for (unsigned int it=0; it<tagindex.size(); it++){
      unsigned int itag=tagindex[it];
      std::vector<std::pair<int,float>>  mupairs= RecoL1tag_Pair((*(&l1mubx)),(*(&l1muqual)),11,etareco[itag],phireco[itag],(*(&l1mueta)),(*(&l1muphi)));
      if(mupairs.size()==0) continue;
      std::sort(mupairs.begin(),mupairs.end(),sort_vintpair_lowHigh);
      if (mupairs[0].second>tagCuts.maxdrL1RecoCone()) continue;
      l1tagpair.emplace_back(make_pair(mupairs[0].first,itag));
    }
 
    if (l1tagpair.size()==0) continue; // if no l1 mu matched pass it
    for(auto it: l1tagpair){
      unsigned int itag=it.second, il1=it.first;
      // fill with tag branches
      pt_tag=ptreco[itag]; 
      eta_tag=etareco[itag]; 
      phi_tag=phireco[itag]; 
      isotag=isoreco[itag]; 
      pt_l1tag=l1mupt[il1];
      eta_l1tag=l1mueta[il1]; 
      phi_l1tag=l1muphi[il1];
      DRtagl1=matchDR(etareco[itag],phireco[itag],l1mueta[il1],l1muphi[il1]);
      TLorentzVector muTag; 
      muTag.SetPtEtaPhiM(ptreco[itag],etareco[itag],phireco[itag],0.105);
      // loop on probes
      for (unsigned int ip=0; ip<probeindex.size(); ip++){
        unsigned int iprobe=probeindex[ip];
        if (itag==iprobe) continue;
        if (chargereco[itag]==chargereco[iprobe]) continue;
        TLorentzVector muProbe; 
        muProbe.SetPtEtaPhiM(ptreco[iprobe],etareco[iprobe],phireco[iprobe],0.105);
        // cuts on tag & probe pair
        if ((muTag+muProbe).M()<probeCuts.minMll() || (muTag+muProbe).M()>probeCuts.maxMll())
           continue;
        if ( (TF=="bmtf" || TF=="omtf" || TF=="emtf") && matchDR(muProbe.Eta(),muProbe.Phi(),muTag.Eta(),muTag.Phi())<(tagCuts.maxdrL1RecoCone()*2))
           continue;
        // for muons of TF use DR to make sure tag is away from probe
        // foe ugmt muons just remove tha tag muon from collection

        pt_probe.push_back(ptreco[iprobe]); 
        eta_probe.push_back(etareco[iprobe]); 
        phi_probe.push_back(phireco[iprobe]); 
        isoprobe.push_back(isoreco[iprobe]);
        std::vector<std::pair<struct l1muon,float>>  mupairs;

        // match muons from TF or ugmt
        if (TF=="bmtf")
           mupairs= RecoTF_Pairs(bmtf->tfMuonBx,bmtf->tfMuonHwQual,etareco[iprobe],phireco[iprobe],bmtf->tfMuonHwPt,bmtf->tfMuonHwEta,bmtf->tfMuonGlobalPhi);
        else if (TF=="omtf")
           mupairs= RecoTF_Pairs(omtf->tfMuonBx,omtf->tfMuonHwQual,etareco[iprobe],phireco[iprobe],omtf->tfMuonHwPt,omtf->tfMuonHwEta,omtf->tfMuonGlobalPhi);
        else if (TF=="emtf")
           mupairs= RecoTF_Pairs(emtf->tfMuonBx,emtf->tfMuonHwQual,etareco[iprobe],phireco[iprobe],emtf->tfMuonHwPt,emtf->tfMuonHwEta,emtf->tfMuonGlobalPhi);
        else
           mupairs= RecoL1_Pairs(il1,(*(&l1mubx)),(*(&l1muqual)),etareco[iprobe],phireco[iprobe],(*(&l1mupt)),(*(&l1mueta)),(*(&l1muphi)));     

        if(mupairs.size()>0) {
           std::sort(mupairs.begin(),mupairs.end(),sort_vpair_lowHigh);
           if (mupairs[0].second>probeCuts.maxdrL1RecoCone()) continue;
           DRprobel1.push_back(mupairs[0].second);
           pt_l1probe.push_back(mupairs[0].first.pt);
           eta_l1probe.push_back(mupairs[0].first.eta);
           phi_l1probe.push_back(mupairs[0].first.phi);
           qual_l1probe.push_back(mupairs[0].first.qual);
        }
   }
  }
  t1->Fill();
}

t1->Write();

return 0;
}
