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
#include "muon_class.h"

float global_eta(float local_eta){
return local_eta*0.010875;
}


float global_phi(float emu_phi){
        float temp=emu_phi/576*6.28-3.14*15/180;
        if (temp>3.14) temp=temp-6.28;
        return temp;
        }


float DPhi(double phi1,double phi2){
float temp=phi1-phi2;
if (temp>3.14) temp=temp-6.28;
if (temp<-3.14) temp=temp+6.28;
return temp;
}

float matchDR(float eta1,float phi1,float eta2,float phi2){
float result=TMath::Sqrt((eta1-eta2)*(eta1-eta2)+DPhi(phi1,phi2)*DPhi(phi1,phi2));
return result;
}
bool sort_vpair_lowHigh(const std::pair<int,float>& a, const std::pair<int,float>& b){
  return a.second<b.second;
}

struct l1match_muon muon_match(float geta, float gphi, std::vector<short> eta, std::vector<short> phi, std::vector<short> pt, std::vector<short> qual, std::vector<short> bx ){
 std::vector<std::pair<int,float>>  mupairs;
for(int i=0; i<pt.size(); i++){
  float neta=global_eta(eta.at(i));
  float nphi=global_phi(phi.at(i));
  if (bx.at(i)!=0) continue;
  if (matchDR(geta,gphi,neta,nphi)>1) continue;
  if (qual.at(i)<11) continue;
//    cout<<" gen eta "<<geta<<" phi "<<gphi<<" l1 "<<neta<<" "<<nphi<<endl;
  //  cout<<" bare phi "<<phi.at(i)<<endl;
     std::pair<int,float> temp(i,matchDR(geta,gphi,neta,nphi));
     mupairs.push_back(temp);
}
struct l1match_muon resmu;
if(mupairs.size()==0){
 resmu={.pt=-9, .eta=-9, .phi=-9, .qual=-9, .DR=-9};
}
else{
std::sort(mupairs.begin(),mupairs.end(),sort_vpair_lowHigh);
int index=mupairs[0].first;
resmu={.pt=static_cast<float>(pt.at(index))/2, .eta=global_eta(static_cast<float>(eta.at(index))), .phi=global_phi(static_cast<float>(phi.at(index))), .qual=static_cast<float>(qual.at(index)), .DR=mupairs[0].second};
}
return resmu;
}
struct l1match_muon gmt_match(float geta, float gphi,TTreeReaderArray<float> pt, TTreeReaderArray<float> eta, TTreeReaderArray<float> phi,  TTreeReaderArray<unsigned short> qual, TTreeReaderArray<short> bx ){
 std::vector<std::pair<int,float>>  mupairs;
for(int i=0; i<sizeof(pt); i++){
  if (bx[i]!=0) continue;
  if (matchDR(geta,gphi,eta[i],phi[i])>1) continue;
  if (qual[i]<11) continue;
//    cout<<" gen eta "<<geta<<" phi "<<gphi<<" l1 "<<neta<<" "<<nphi<<endl;
  //  cout<<" bare phi "<<phi.at(i)<<endl;
     std::pair<int,float> temp(i,matchDR(geta,gphi,eta[i],phi[i]));
     mupairs.push_back(temp);
}
struct l1match_muon resmu;
if(mupairs.size()==0){
 resmu={.pt=-9, .eta=-9, .phi=-9, .qual=-9, .DR=-9};
}
else{
std::sort(mupairs.begin(),mupairs.end(),sort_vpair_lowHigh);
int index=mupairs[0].first;
resmu={.pt=static_cast<float>(pt[index]), .eta=global_eta(static_cast<float>(eta[index])), .phi=global_phi(static_cast<float>(phi[index])), .qual=static_cast<float>(qual[index]), .DR=mupairs[0].second};
}
return resmu;
}


struct l1match_muon muon_matchTP(float geta, float gphi, std::vector<short> eta, std::vector<short> phi, std::vector<short> pt, std::vector<short> qual, std::vector<short> bx ){
 
 std::vector<std::pair<int,float>>  mupairs;
for(int i=0; i<pt.size(); i++){
  float neta=global_eta(eta.at(i));
  float nphi=global_phi(phi.at(i));
  if (bx.at(i)!=0) continue;
  if (matchDR(geta,gphi,neta,nphi)>1) continue;
  if (qual.at(i)<11) continue;
//    cout<<" gen eta "<<geta<<" phi "<<gphi<<" l1 "<<neta<<" "<<nphi<<endl;
  //  cout<<" bare phi "<<phi.at(i)<<endl;
     std::pair<int,float> temp(i,matchDR(geta,gphi,neta,nphi));
     mupairs.push_back(temp);
}
struct l1match_muon resmu;
if(mupairs.size()==0){
 resmu={.pt=-9, .eta=-9, .phi=-9, .qual=-9, .DR=-9};
}
else{
std::sort(mupairs.begin(),mupairs.end(),sort_vpair_lowHigh);
int index=mupairs[0].first;
resmu={.pt=static_cast<float>(pt.at(index))/2, .eta=global_eta(static_cast<float>(eta.at(index))), .phi=global_phi(static_cast<float>(phi.at(index))), .qual=static_cast<float>(qual.at(index)), .DR=mupairs[0].second};
}
 return resmu;
  }


