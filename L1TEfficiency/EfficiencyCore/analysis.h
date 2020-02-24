/* Fuctions for L1T analysis package
 * Author: George Karathanasis, georgios.karathanasis@cern.ch
 * 
 *
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
bool sort_vpair_lowHigh(const std::pair<struct l1muon,float>& a, const std::pair<struct l1muon,float>& b){
  return a.second<b.second;
}

bool sort_vintpair_lowHigh(const std::pair<int,float>& a, const std::pair<int,float>& b){
  return a.second<b.second;
}

std::vector<std::pair<int,float>>  RecoL1tag_Pair(TTreeReaderArray<short>& l1bx,TTreeReaderArray<unsigned short>& l1qual,unsigned short qual,float etareco,float phireco,TTreeReaderArray<float>& l1eta,TTreeReaderArray<float>& l1phi){
  std::vector<std::pair<int,float>> pairs;
  for(unsigned int i=0; i<l1bx.GetSize (); i++){
   if (l1bx[i]!=0) continue;
   if (l1qual[i]<qual) continue;
   std::pair<int,float> temp(i,matchDR(etareco,phireco,l1eta[i],l1phi[i]));
   pairs.push_back(temp);
  }
  return pairs;
}

std::vector<std::pair<struct l1muon,float>>  RecoL1_Pair(int itag,TTreeReaderArray<short>& l1bx,TTreeReaderArray<unsigned short>& l1qual,unsigned short qual,float etareco,float phireco,TTreeReaderArray<float>& l1pt,TTreeReaderArray<float>& l1eta,TTreeReaderArray<float>& l1phi){
  std::vector<std::pair<struct l1muon,float>> pairs;
  for(unsigned int i=0; i<l1bx.GetSize (); i++){
   if (i==itag) continue;
   if (l1bx[i]!=0) continue;
   if (l1qual[i]<qual) continue;
   struct l1muon l1muon_obj;
   l1muon_obj={.pt=l1pt[i], .eta=l1eta[i], .phi=l1phi[i], .qual=static_cast<float>(l1qual[i])};
   std::pair<struct l1muon,float> temp(l1muon_obj,matchDR(etareco,phireco,l1eta[i],l1phi[i]));
   pairs.push_back(temp);
  }
  return pairs;
}



std::vector<std::pair<struct l1muon,float>>  RecoL1_Pairs(int itag, TTreeReaderArray<short>& l1bx, TTreeReaderArray<unsigned short>& l1qual, float etareco, float phireco,TTreeReaderArray<float>& l1pt, TTreeReaderArray<float>& l1eta,TTreeReaderArray<float>& l1phi){
   std::vector<std::pair<struct l1muon,float>>  mupairs;
   if (mupairs.size()==0)
      mupairs= RecoL1_Pair(itag,l1bx,l1qual,7,etareco,phireco,l1pt,l1eta,l1phi);
   if (mupairs.size()==0)
      mupairs= RecoL1_Pair(itag,l1bx,l1qual,3,etareco,phireco,l1pt,l1eta,l1phi);
   if (mupairs.size()==0)
      mupairs= RecoL1_Pair(itag,l1bx,l1qual,-1,etareco,phireco,l1pt,l1eta,l1phi);
   return mupairs;
}

std::vector<std::pair<struct l1muon,float>>  RecoTF_Pair(std::vector<short> l1bx,std::vector<short> l1qual,unsigned short qual, float etareco, float phireco,std::vector<short> l1pt, std::vector<short> l1eta,std::vector<short> l1phi){
  std::vector<std::pair<struct l1muon,float>> pairs;
  for(unsigned int i=0; i<l1bx.size(); i++){
   if (l1bx[i]!=0) continue;
   if (l1qual[i]<qual) continue;
   struct l1muon l1muon_obj;
   l1muon_obj={.pt=static_cast<float>(l1pt[i]*0.5), .eta=global_eta(static_cast<float>(l1eta[i])), .phi=global_phi(static_cast<float>(l1phi[i])), .qual=static_cast<float>(l1qual[i])};
   std::pair<struct l1muon,float> temp(l1muon_obj,matchDR(etareco,phireco,global_eta(static_cast<float>(l1eta[i])),global_phi(static_cast<float>(l1phi[i]))));
   pairs.push_back(temp);
  }
  return pairs;
}


std::vector<std::pair<struct l1muon,float>>  RecoTF_Pairs(std::vector<short> l1bx,std::vector<short> l1qual, float etareco, float phireco,std::vector<short> l1pt, std::vector<short> l1eta,std::vector<short> l1phi){
   std::vector<std::pair<struct l1muon,float>>  mupairs;
   if (mupairs.size()==0)
      mupairs= RecoTF_Pair(l1bx,l1qual,7,etareco,phireco,l1pt,l1eta,l1phi);
   if (mupairs.size()==0)
      mupairs= RecoTF_Pair(l1bx,l1qual,3,etareco,phireco,l1pt,l1eta,l1phi);
   if (mupairs.size()==0)
      mupairs= RecoTF_Pair(l1bx,l1qual,-1,etareco,phireco,l1pt,l1eta,l1phi);
   return mupairs;
}


