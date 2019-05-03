#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include "plot_tool.h"


int DefaultColor(int j,int i){
  if (j-i==1) return 2;
  else if (j-i==2) return 4;
  else if (j-i==3) return 6;
  else if (j-i==4) return 8;
  else if (j-i==5) return 9;
  else return j;
}

//   else if (value[0]=="Errors")
//   tSimple.C

int plotSimple( std::string plots, TString ntuple,TString path){


 TChain * ccdata=new TChain("l1UpgradeTree/L1UpgradeTree");
 TChain * ccemu=new TChain("l1UpgradeEmuTree/L1UpgradeTree");
 ccdata->Add(ntuple+"/*.root"); ccemu->Add(ntuple+"/*.root");
 gStyle->SetOptStat(0); std::string line;
 cout<<ccdata->GetEntries()<<endl;
 std::vector<TH1F*> hplot;
 std::vector<TString> canvasname; std::vector<std::string> kwds;
 std::ifstream infile(plots);
 std::vector<bool> grid,logY,logX;
//create histo
 while (std::getline(infile, line)){
  vector<string> tokens;   
  boost::split(tokens,line,boost::is_any_of(":"));
  if (tokens[0]=="#" || tokens[0]==" ") 
    continue;
  if (tokens.size()<9) {
    cout<<"requested plot: "<<line<<" has less options that the minimum- skipping please check and replot"<<endl;
    continue;
   }
   if (tokens[2]=="None") tokens[2]="1>0";
   cout<<"plotting... "<<tokens[0]<<" histo "<<endl;
   TH1F *temp_data,*temp_emu;
   if (tokens[3].find("data") != std::string::npos){
     temp_data=gethisto(ccdata,tokens[1],tokens[2],"hdata_"+tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));
     hplot.push_back(temp_data); canvasname.push_back(tokens[8]);
     TString titleX=tokens[4]; temp_data->GetXaxis()->SetTitle(titleX);
     std::string options=""; bool tempX=false,tempY=false,tempGrid=false;
     for(unsigned int itoken=9; itoken<tokens.size(); ++itoken){
       options+=tokens[itoken]+"_";
       if (tokens[itoken]=="Norm") temp_data->Scale(1/temp_data->Integral());
       if (tokens[itoken]=="Grid") tempGrid=true;
       if (tokens[itoken]=="LogX") tempX=true;
       if (tokens[itoken]=="LogY") tempY=true;
     }
     grid.push_back(tempGrid); logY.push_back(tempY); logX.push_back(tempX);
     kwds.push_back(options);
   }
   if (tokens[3].find("emu") != std::string::npos){
     temp_emu=gethisto(ccemu,tokens[1],tokens[2],"hemu_"+tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));
     hplot.push_back(temp_emu); canvasname.push_back(tokens[8]);
     TString titleX=tokens[4]; temp_emu->GetXaxis()->SetTitle(titleX);
     std::string options=""; bool tempX=false,tempY=false,tempGrid=false;
     for(unsigned int itoken=9; itoken<tokens.size(); ++itoken){
       options+=tokens[itoken]+"_";
       if (tokens[itoken]=="Norm") temp_emu->Scale(1/temp_emu->Integral());
       if (tokens[itoken]=="Grid") tempGrid=true;
       if (tokens[itoken]=="LogX") tempX=true;
       if (tokens[itoken]=="LogY") tempY=true;
     }
     kwds.push_back(options);
     grid.push_back(tempGrid); logY.push_back(tempY); logX.push_back(tempX);
   }
 }
//plot canvases
 for (int i=0; i<canvasname.size(); i++){
   if (canvasname[i]=="Canvas_name_already_used_action_skipping") 
     continue;
   TCanvas * c1=new TCanvas(canvasname[i],canvasname[i],700,700);
   hplot[i]->Draw();
   //default style
   hplot[i]->SetTitle("#font[22]{CMS} #font[12]{Preliminary}");
   hplot[i]->GetXaxis()->SetTitleSize(0.065);
   hplot[i]->GetXaxis()->SetTitleOffset(0.7);
   hplot[i]->GetYaxis()->SetTitleSize(0.045);
   hplot[i]->GetYaxis()->SetTitleOffset(0.8);
   hplot[i]->SetLineWidth(3); hplot[i]->SetLineColor(1);
//   if (kwds.size()!=0) transform( heff[i],kwds);
   TLegend * leg =new TLegend(0.7,0.1,0.9,0.3);
   leg->AddEntry(hplot[i],hplot[i]->GetName());
   bool printLeg=false;
   for (int j=i+1; j<canvasname.size(); j++){
     if (canvasname[i]!=canvasname[j]) continue;
     hplot[j]->Draw("sames");
     canvasname[j]="Canvas_name_already_used_action_skipping";
     printLeg=true;
     hplot[j]->SetLineWidth(3);
     hplot[j]->SetLineColor(DefaultColor(j,i));
     leg->AddEntry(hplot[j],hplot[j]->GetName()); 
   }
   if (printLeg) leg->Draw("sames");
   if (grid[i]) c1->SetGrid();
   if (logX[i]) c1->SetLogx();
   if (logY[i]) c1->SetLogy();
   c1->SaveAs(canvasname[i]+".png");
   canvasname[i]="Canvas_name_already_used_action_skipping";
 }


 return 0;}
