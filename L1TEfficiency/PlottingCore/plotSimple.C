/* L1T Analysis plotting script
 * module: Plain plotter
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


TLatex cms_latex(){
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

//   else if (value[0]=="Errors")
//   tSimple.C

int plotSimple( std::string plots, TString ntuple,TString path){

  TChain * ccdata=new TChain("l1UpgradeTree/L1UpgradeTree");
  TChain * ccemu=new TChain("l1UpgradeEmuTree/L1UpgradeTree");
  cout << "ntuple" << ntuple << endl;
  ccdata->Add(ntuple); 
  ccemu->Add(ntuple);

  gStyle->SetOptStat(0); 
  std::string line;

  cout<<"Running on "<<ccdata->GetEntries()<<" evts"<<endl;

  //vectors
  std::vector<TH1F*> hplot;
  std::vector<TString> canvasname; 
  std::vector<std::string> kwds;
  std::vector<TString> legs;
  std::ifstream infile(plots);
  std::vector<bool> grid,logY,logX;

  //create histo
  while (std::getline(infile, line)){

    vector<string> tokens;   

    boost::split(tokens,line,boost::is_any_of(":"));
    //pass plots commented
    if (tokens.size()==1) continue;
    if (tokens[0]=="#") continue;

    // protection
    if (tokens.size()<9) {
       cout<<"ERROR requested plot: "<<tokens[0]<<" less options that mandatory- SKIPPING "<<endl;
       continue;  }

    //initialize
    if (tokens[2]=="None") tokens[2]="1>0";
    cout<<"plotting... "<<tokens[0]<<" histo "<<endl;
   
    // create plots
    TH1F *temp_data,*temp_emu;
    // if run on data
    if (tokens[3].find("data") != std::string::npos){
      temp_data=gethisto(ccdata,tokens[1],tokens[2],"hdata_"+tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));

      hplot.push_back(temp_data); 
      canvasname.push_back(tokens[8]);

      TString titleX=tokens[4]; 
      temp_data->GetXaxis()->SetTitle(titleX);

      std::string options=""; 
      vector<string> temp_leg;
      bool tempX=false,tempY=false,tempGrid=false;

      // save optional arguments
      for(unsigned int itoken=9; itoken<tokens.size(); ++itoken){
        cout<<"   selected option "<<tokens[itoken]<<endl;
        if ( tokens[itoken]!="Norm" && tokens[itoken]!="NoGrid"
             && tokens[itoken]!="LogX" && tokens[itoken]!="LogY"
             && tokens[itoken].find("leg")== std::string::npos){
           if (options=="")
              options=tokens[itoken];
           else 
              options+="_"+tokens[itoken];
        }
        if (tokens[itoken].find("leg")!= std::string::npos)
           boost::split(temp_leg,tokens[itoken],boost::is_any_of("="));
     
        if (tokens[itoken]=="Norm") 
           temp_data->Scale(1/temp_data->Integral());
        if (tokens[itoken]=="NoGrid") 
           tempGrid=true;
        if (tokens[itoken]=="LogX") 
           tempX=true;
        if (tokens[itoken]=="LogY")
           tempY=true;
      }
      cout<<"options "<<options<<endl; 
      grid.push_back(tempGrid);
      logY.push_back(tempY);
      logX.push_back(tempX);
      kwds.push_back(options);
      if (temp_leg.size()>0)
        legs.push_back(temp_leg[1]);
      else
        legs.push_back(tokens[0]);
    }   

    //if run on emulator
    
    if (tokens[3].find("emu") != std::string::npos){
       vector<string> temp_leg;
       temp_emu=gethisto(ccemu,tokens[1],tokens[2],"hemu_"+tokens[0],std::stoi(tokens[5]),std::stof(tokens[6]),std::stof(tokens[7]));

       hplot.push_back(temp_emu); 
       canvasname.push_back(tokens[8]);

       TString titleX=tokens[4]; 
       temp_emu->GetXaxis()->SetTitle(titleX);
       std::string options=""; 
       bool tempX=false,tempY=false,tempGrid=false;

       //save optional arguments
       for(unsigned int itoken=9; itoken<tokens.size(); ++itoken){
          cout<<"   selected option "<<tokens[itoken]<<endl;
          if ( tokens[itoken]!="Norm" && tokens[itoken]!="NoGrid" 
             && tokens[itoken]!="LogX" && tokens[itoken]!="LogY" 
             && tokens[itoken]!="leg"){
           if (options=="")
              options=tokens[itoken];
           else 
              options+="_"+tokens[itoken];
        }
         if (tokens[itoken]=="Norm") 
            temp_emu->Scale(1/temp_emu->Integral());
         if (tokens[itoken]=="NoGrid") 
            tempGrid=true;
         if (tokens[itoken]=="LogX") 
            tempX=true;
         if (tokens[itoken]=="LogY") 
            tempY=true;
      }
      kwds.push_back(options);
      grid.push_back(tempGrid); 
      logY.push_back(tempY); 
      logX.push_back(tempX);
      if (temp_leg.size()>0)
        legs.push_back(temp_leg[1]);
      else
        legs.push_back(tokens[0]);
    }
 }
 
 
 //create canvases
 for (int i=0; i<canvasname.size(); i++){
   if (canvasname[i]=="Canvas_name_already_used_action_skipping") 
     continue;
   TCanvas * c1=new TCanvas(canvasname[i],canvasname[i],700,700);
   hplot[i]->Draw("E1");
   hplot[i]->SetLineWidth(3); 
   hplot[i]->SetLineColor(2); 

   TLatex cms_label=cms_latex();
   TLatex header=head();
   if (kwds[i]!=""){
     cout<<kwds[i]<<endl;
     transform( hplot[i],kwds[i]);
   }
   TLegend * leg =new TLegend(0.7,0.7,0.9,0.9);
   leg->AddEntry(hplot[i],legs[i]);
   bool printLeg=false;
   for (int j=i+1; j<canvasname.size(); j++){
     if (canvasname[i]!=canvasname[j]) continue;
     hplot[j]->Draw("sames E1");
     canvasname[j]="Canvas_name_already_used_action_skipping";
     printLeg=true;
     hplot[j]->SetLineWidth(3);
     hplot[j]->SetLineColor(DefaultColor(j,i));
     leg->AddEntry(hplot[j],legs[j]); 
     if (kwds[j]!="")
       transform( hplot[j],kwds[j]);
   }
   if (printLeg) leg->Draw("sames");
   if (!grid[i]) c1->SetGrid(0);
   if (logX[i]) c1->SetLogx();
   if (logY[i]) c1->SetLogy();
   c1->SaveAs(path+"/"+canvasname[i]+".png");
   c1->SaveAs(path+"/"+canvasname[i]+".pdf");
   canvasname[i]="Canvas_name_already_used_action_skipping";
 }


 return 0;}
