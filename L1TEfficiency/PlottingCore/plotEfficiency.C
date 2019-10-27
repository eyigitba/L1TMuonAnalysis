/* L1T Analysis plotting script
 * module: Efficiency plotter
 * Author: George Karathanasis georgios.karathanasis@cern.ch
 *
 */

#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include "plot_tool.h"
#include "TLatex.h"


TLatex cms(){
  TLatex cms_label;
  cms_label.SetTextSize(0.04);
  cms_label.DrawLatexNDC(0.16, 0.92, "#bf{ #font[22]{CMS} #font[72]{Preliminary}}");
  return cms_label;
}

TLatex head(){
  TLatex header; 
  header.SetTextSize(0.03);
  header.DrawLatexNDC(0.63, 0.92, "#sqrt{s} = 13 TeV, 2018 p-p ");
  return header; 
}

int DefaultColor(int j,int i){
  if (j-i==1) return 2;
  else if (j-i==2) return 4;
  else if (j-i==3) return 6;
  else if (j-i==4) return 8;
  else if (j-i==5) return 9;
  else return j;
}


int plotEfficiency( std::string plots, TString ntuple, TString path, TString ntupleEmul="none"){

 //read data
 TChain * cc=new TChain("efftree");
 cc->Add(ntuple+"/*.root");
 TChain * ccEmul=new TChain("efftree");
 if (ntupleEmul!="none") ccEmul->Add(ntupleEmul+"/*.root");

 gStyle->SetOptStat(0);
 std::string line;

 cout<<"Running on "<<cc->GetEntries()<<" evts "<<endl;

 //plot containers
 std::vector<TString> canvasname;
 std::vector<std::string> kwds;
 std::vector<TString> legs;
 std::vector<TGraphAsymmErrors*> errors;
 std::ifstream infile(plots);

 // cosmetic options
 std::vector<bool> grid,logY,logX;
 //read plots from file
 while (std::getline(infile, line)){
   vector<string> tokens;

   boost::split(tokens,line,boost::is_any_of(":"));
       
   //skip empty lines
   if (tokens.size()==1) continue;
   //pass lines starting with #
   if (tokens[0]=="#") continue;
   
   // protection
   if (tokens.size()<9) {
      cout<<"ERROR requested plot: "<<tokens[0]<<" less options that mandatory- SKIPPING "<<endl;
      continue;
   }

   // initialize cuts
   if (tokens[2]=="none") tokens[2]="1>0";
   if (tokens[3]=="none") tokens[3]="1>0";

   cout<<"plotting... "<<tokens[0]<<" histo "<<endl;

   //create efficiencies
   TH1F *temp_den=gethisto(cc,tokens[1],tokens[2],"hden_"+tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));
   TH1F *temp_num=gethisto(cc,tokens[1],tokens[2]+" && "+tokens[3],tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));
   TH1F *temp_denEmul,*temp_numEmul;

   // if there is emulator vs data option fill emulator
   if (ntupleEmul!="none"){
     temp_denEmul=gethisto(ccEmul,tokens[1],tokens[2],"hdenEmul_"+tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));
     temp_numEmul=gethisto(ccEmul,tokens[1],tokens[2]+" && "+tokens[3],"hEmul_"+tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));
   }   

   std::string options=""; 
   vector<string> temp_leg;
   bool tempX=false,tempY=false,tempGrid=true;

   
   // Divide histograms
   TGraphAsymmErrors * error=new TGraphAsymmErrors(temp_num,temp_den);
   TGraphAsymmErrors * errorEmul=NULL;
   if (ntupleEmul!="none")
        errorEmul=new TGraphAsymmErrors(temp_numEmul,temp_denEmul);

   //sort options
   for(unsigned int itoken=9; itoken<tokens.size(); ++itoken){
    cout<<"   selected option "<<tokens[itoken]<<endl;
    if ( tokens[itoken]!="NoGrid" && tokens[itoken]!="LogX"
       && tokens[itoken]!="LogY"
       && tokens[itoken].find("leg")== std::string::npos ){
         if (options=="") 
           options=tokens[itoken];
         else
          options+="_"+tokens[itoken];
    }

    if (tokens[itoken].find("leg")!= std::string::npos)
      boost::split(temp_leg,tokens[itoken],boost::is_any_of("="));
    
    if (tokens[itoken]=="NoGrid") tempGrid=false;
    if (tokens[itoken]=="LogX") tempX=true;
    if (tokens[itoken]=="LogY") tempY=true;
   }

   canvasname.push_back(tokens[8]); 
   TString titleX=tokens[4];

   //save plotting options per plot
   grid.push_back(tempGrid); 
   logX.push_back(tempX); 
   logY.push_back(tempY);
   error->GetXaxis()->SetTitle(titleX);
   error->GetYaxis()->SetTitle("L1T Efficiency");
   error->GetYaxis()->SetRangeUser(0.00001,1);

   if (temp_leg.size()>0) 
      legs.push_back(temp_leg[1]);
   else if (ntupleEmul=="none" && temp_leg.size()==0)
      legs.push_back(tokens[0]);
   else if (ntupleEmul!="none" && temp_leg.size()==0)
      legs.push_back("Data"); 

   kwds.push_back(options);
   errors.push_back(error);
   if (ntupleEmul!="none"){
     canvasname.push_back(tokens[8]); 
     legs.push_back("Emulator");
     kwds.push_back(options);
     errors.push_back(errorEmul);
  }
 }

 for (int i=0; i<canvasname.size(); i++){
   if (canvasname[i]=="Canvas_name_already_used_action_skipping") 
       continue;
   //create canvas and save histos
   TCanvas * c1=new TCanvas(canvasname[i],canvasname[i],700,700);
   errors[i]->Draw("A P");
   errors[i]->SetLineWidth(3);
   TLatex cms_label=cms();
   TLatex header=head();

   if (kwds[i]!="")
     transform( errors[i],kwds[i]);

   TLegend * leg =new TLegend(0.7,0.1,0.9,0.3);
   leg->AddEntry(errors[i],legs[i]);
   bool printLeg=false;

   //put histos with same cnvas name in same plot
   for (int j=i+1; j<canvasname.size(); j++){
     if (canvasname[i]!=canvasname[j]) continue;
     errors[j]->Draw("sames P");
     canvasname[j]="Canvas_name_already_used_action_skipping";
     printLeg=true;
     errors[j]->SetLineWidth(3);
     errors[j]->SetLineColor(DefaultColor(j,i));
     if (kwds[j]!="")
       transform( errors[j],kwds[j]);
     leg->AddEntry(errors[j],legs[j]);
   }

   if (printLeg) leg->Draw("sames");
   if (!grid[i]) c1->SetGrid(0);
   if (logX[i]) c1->SetLogx();
   if (logY[i]) c1->SetLogy();

   c1->SaveAs(path+"/"+canvasname[i]+".png");
   c1->SaveAs(path+"/"+canvasname[i]+".pdf");
   canvasname[i]="Canvas_name_already_used_action_skipping";

 }


 return 0;
 }
