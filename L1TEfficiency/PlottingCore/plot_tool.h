/* L1T Analysis package plotting tools
 * module: plotting functions
 * Author: George Karathanasis georgios.karathanasis@cern.ch
 *
 */

#include "TCanvas.h"
#include "TLegend.h"
#include "TH1.h"
#include "TString.h"
#include <boost/algorithm/string.hpp>


  
Double_t turnon_func(Double_t *x, Double_t *par){
   double halfpoint = par[0];
   double slope = par[1];
   double plateau = par[2];
   double offset = 0;
   double pt = TMath::Max(x[0],0.000001);
   double arg = (pt-halfpoint)/(TMath::Sqrt(pt)*slope);
   double fitval = offset+0.5*plateau*(1+TMath::Erf(arg));
   return fitval;
}

 auto myErf2Lambda(double* x, double* p) {
           return (TMath::Erf((x[0]-p[0])/p[1]) + 1)/2 * p[2]; };


void transform( TGraphAsymmErrors * heff, std::string kwds){

   std::vector<std::string> cuts;
   boost::split(cuts,kwds,boost::is_any_of("_"));
   for ( std::string cut : cuts){
     std::vector<std::string> value;
     boost::split(value,cut,boost::is_any_of("="));
     TString tstr=value[1];
     for (int ist=0; ist<value.size(); ist++)
     if (value[0]=="LineColor") heff->SetLineColor(std::stoi(value[1]));   
     else if (value[0]=="LineWidth") heff->SetLineWidth(std::stoi(value[1]));
     else if (value[0]=="XLabelSize"){
        heff->GetXaxis()->SetTitleSize(std::stof(value[1]));
     }
     else if (value[0]=="YLabelSize") heff->GetYaxis()->SetTitleSize(std::stof(value[1]));
     else if (value[0]=="YTitle") heff->GetYaxis()->SetTitle(tstr);
     else if (value[0]=="Fit"){
       TString function="gauss";
       TF1 * f1;
       std::vector<std::string> range;
       boost::split(range,value[1],boost::is_any_of(","));

       if (value[1].find("Gauss")!= std::string::npos)
         f1=new TF1("f1","gauss",std::stof(range[1]),std::stof(range[1]));

       else if (value[1].find("Erf")!= std::string::npos){
         f1 = new TF1("f1", myErf2Lambda, std::stof(range[1]),std::stof(range[2]), 3 );
         f1->SetParameter(0, 5.);
         f1->SetParameter(1, 3.);
         f1->SetParameter(2, 0.8);
       }
       else if (value[1].find("Exp")!= std::string::npos)
         f1=new TF1("f1","expo",std::stof(range[0]),std::stof(range[1])); 

       else
         f1=new TF1("f1","pol2",std::stof(range[0]),std::stof(range[1]));
      
       heff->Fit("f1","r");       
     }
    else cout<<"uknown option "<<value[0]<<" ommiting "<<endl;
  }
}



void transform( TH1F * heff, std::string kwds){

   std::vector<std::string> cuts;
   boost::split(cuts,kwds,boost::is_any_of("_"));
   for ( std::string cut : cuts){
     std::vector<std::string> value;
     boost::split(value,cut,boost::is_any_of("="));
     TString tstr=value[1];
     for (int ist=0; ist<value.size(); ist++)
       std::cout<<"index "<<ist<<" val "<<value[ist]<<endl;
     if (value[0]=="LineColor") heff->SetLineColor(std::stoi(value[1]));   
     else if (value[0]=="LineWidth") heff->SetLineWidth(std::stoi(value[1]));
     else if (value[0]=="XLabelSize"){
        heff->GetXaxis()->SetTitleSize(std::stof(value[1]));
     }
     else if (value[0]=="YLabelSize") heff->GetYaxis()->SetTitleSize(std::stof(value[1]));
     else if (value[0]=="YTitle") heff->GetYaxis()->SetTitle(tstr);
     else if (value[0]=="Fit"){
       TString function="gauss";
       TF1 * f1;
       std::vector<std::string> range;
       boost::split(range,value[1],boost::is_any_of(","));

       if (value[1].find("Gauss")!= std::string::npos)
         f1=new TF1("f1","gauss",std::stof(range[1]),std::stof(range[1]));

       else if (value[1].find("Erf")!= std::string::npos){
         f1 = new TF1("f1", myErf2Lambda, std::stof(range[1]),std::stof(range[2]), 3 );
         f1->SetParameter(0, 5.);
         f1->SetParameter(1, 3.);
         f1->SetParameter(2, 0.8);
       }
       else if (value[1].find("Exp")!= std::string::npos)
         f1=new TF1("f1","expo",std::stof(range[0]),std::stof(range[1])); 

       else
         f1=new TF1("f1","pol2",std::stof(range[0]),std::stof(range[1]));
      
       heff->Fit("f1","r");       
     }
     else if (value[0]=="Rate"){
       std::vector<std::string> input;
       boost::split(input,value[1],boost::is_any_of(","));
       TH1F* hclone=(TH1F*) heff->Clone();
       for (int ibin=0; ibin<hclone->GetNbinsX(); ibin++){
          float temp=hclone->Integral(ibin,hclone->GetNbinsX());
          temp=11245*temp*std::stof(input[0])/std::stof(input[1]);
          heff->SetBinContent(ibin,temp);
       }
    }
    else cout<<"uknown option "<<value[0]<<" ommiting "<<endl;
  }
}





TH1F * gethisto(TChain * cc,std::string var,std::string cuts, std::string name, int bins, float start, float end){
  TString name2(name);
 TH1F * htemp=new TH1F(name2,"#font[22]{CMS} #font[12]{Preliminary}",bins,start,end);
 TString var2(var+">>"+name);
 TString cuts2(cuts);
 cc->Draw(var2,cuts2);
 return htemp;
}

void addregion(TChain * cc,TString var,TString cuts, TString name){
 cc->Draw(var+">>+"+name,cuts);
}



