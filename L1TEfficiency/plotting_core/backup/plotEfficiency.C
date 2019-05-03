#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include "plot_tool.h"


int plotEfficiency( std::string plots, TString ntuple,TString path){
  TChain * cc=new TChain("efftree");
  cc->Add(ntuple+"/*.root");
std::string line;
 cout<<cc->GetEntries()<<endl;
 std::vector<TH1F*> hnum,hden;
 std::vector<TString> title;
std::ifstream infile(plots);
 while (std::getline(infile, line))
{
  cout<<"new line"<<endl;
  vector<string> tokens;

     istringstream iss(line);
       cout<<line<<endl;
       /*copy(istream_iterator<string>(iss),
     istream_iterator<string>(),
     back_inserter(tokens));*/
       std::string token;
       /* while (std::getline(iss, token, ":")) {
        tokens.push_back(token);
    }*/
       boost::split(tokens,line,boost::is_any_of(":"));
       if (tokens[0]=="#" || tokens[0]==" ") continue;
        TH1F *temp=gethisto(cc,tokens[1],tokens[2],"h"+tokens[0],std::stoi(tokens[4]),std::stof(tokens[5]),std::stof(tokens[6]));
	if (tokens[7]=="NUM") hnum.push_back(temp);
      	if (tokens[7]=="DEN") hden.push_back(temp);
       TCanvas * ctemp=canvas_1plot(temp,"c"+tokens[0],false,tokens[3],"L1 Efficiency");
        TString titl(tokens[0]);
        title.push_back(titl);
//        ctemp->SaveAs(path+"/"+titl+".png");
    // process pair (a,canvas_1plot();b)
       /* for (int i=0; i<tokens.size(); i++){
   if (tokens[i]==":") continue;
   cout<<tokens[i]<<endl;
   }*/
	
}
cout<<hnum.size()<<endl;
for (int i=0; i<hnum.size(); i++){
  cout<<i<<endl;
TH1F *htnum=hnum[i];
TH1F *htden=hden[i];
 	 htnum->Divide(htden);
   TCanvas * ctemp=canvas_1plot(htnum,"c"+title[i],false,"pt","L1 Efficiency");
   
    }

 return 0;}
