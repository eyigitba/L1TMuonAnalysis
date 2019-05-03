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


int plotEfficiency( std::string plots, TString ntuple,TString path){
 TChain * cc=new TChain("efftree");
 cc->Add(ntuple+"/*.root");
 gStyle->SetOptStat(0);
 std::string line;
 cout<<cc->GetEntries()<<endl;
 std::vector<TH1F*> heff;
 std::vector<TString> canvasname;
 std::vector<std::string> kwds;
 std::vector<TString> legs;
 std::vector<TGraphAsymmErrors*> errors;
 std::ifstream infile(plots);
 std::vector<bool> grid,logY,logX;
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
   TH1F *temp_den=gethisto(cc,tokens[1],tokens[2],"hden_"+tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));
   TH1F *temp_num=gethisto(cc,tokens[1],tokens[2]+" && "+tokens[3],tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));
   std::string options=""; vector<string> temp_leg;
   bool tempX=false,tempY=false,tempGrid=false;
   TGraphAsymmErrors * error=NULL;
   for(unsigned int itoken=9; itoken<tokens.size(); ++itoken){
    options+=tokens[itoken]+"_";
    if (tokens[itoken].find("leg")!= std::string::npos)
      boost::split(temp_leg,tokens[itoken],boost::is_any_of("="));
    if(tokens[itoken]=="AddErrors")
      error=new TGraphAsymmErrors(temp_num,temp_den); 
    if (tokens[itoken]=="Grid") tempGrid=true;
    if (tokens[itoken]=="LogX") tempX=true;
    if (tokens[itoken]=="LogY") tempY=true;

   }
   temp_num->Divide(temp_den); heff.push_back(temp_num);
   canvasname.push_back(tokens[8]); TString titleX=tokens[4];
   temp_num->GetXaxis()->SetTitle(titleX);
   temp_num->GetYaxis()->SetTitle("L1T Efficiency");
   temp_num->GetYaxis()->SetRangeUser(0.00001,1);
   if (temp_leg.size()>0) legs.push_back(temp_leg[1]);
   else legs.push_back(tokens[0]);
   kwds.push_back(options);
   errors.push_back(error);
 }
 for (int i=0; i<canvasname.size(); i++){
   if (canvasname[i]=="Canvas_name_already_used_action_skipping") 
     continue;
   TCanvas * c1=new TCanvas(canvasname[i],canvasname[i],700,700);
   heff[i]->Draw();
   //default style
   heff[i]->SetTitle("#font[22]{CMS} #font[12]{Preliminary}");
   if (errors[i]!=NULL){
      errors[i]->Draw("sames");  errors[i]->SetMarkerColor(1);
   }
   heff[i]->GetXaxis()->SetTitleSize(0.065);
   heff[i]->GetXaxis()->SetTitleOffset(0.7);
   heff[i]->GetYaxis()->SetTitleSize(0.045);
   heff[i]->GetYaxis()->SetTitleOffset(0.8);
   heff[i]->SetLineWidth(3); heff[i]->SetLineColor(1);
   if (kwds[i]!="") transform( heff[i],kwds[i]);
   TLegend * leg =new TLegend(0.7,0.1,0.9,0.3);
   leg->AddEntry(heff[i],legs[i]);
   bool printLeg=false;
   for (int j=i+1; j<canvasname.size(); j++){
     if (canvasname[i]!=canvasname[j]) continue;
     heff[j]->Draw("sames");
     if (errors[j]!=NULL){ 
      errors[j]->Draw("sames");  errors[j]->SetMarkerColor(1);
     }
     canvasname[j]="Canvas_name_already_used_action_skipping";
     printLeg=true;
     heff[j]->SetLineWidth(3);
     heff[j]->SetLineColor(DefaultColor(j,i));
     leg->AddEntry(heff[j],legs[j]);
   }
   if (printLeg) leg->Draw("sames");
   if (grid[i]) c1->SetGrid();
   if (logX[i]) c1->SetLogx();
   if (logY[i]) c1->SetLogy();
   c1->SaveAs(canvasname[i]+".png");
   canvasname[i]="Canvas_name_already_used_action_skipping";
 }


 return 0;}
