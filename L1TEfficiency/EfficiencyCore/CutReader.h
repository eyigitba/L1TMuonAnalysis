/* L1T analysis package 
 * module: Cut parser
 * Author: George Karathanasis, georgios.karathanasis@cern.ch
 *
 *
 */

#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <boost/algorithm/string.hpp>


class CutReader{
 
 public:
   CutReader(TString file);   
   ~CutReader();
   float cutReading(float def,std::string cut);
   float maxEta(){ return cutReading(2.4,"MaxEta"); }
   float minEta() {return cutReading(0,"MinEta");}
   float hltIso() {return cutReading(15,"MaxHLTIso");}
   float minPt() {return cutReading(0,"MinPt");}
   float maxHLTDrCone() {return cutReading(3,"MaxHLTDrCone");}
   float maxMll() {return cutReading(1000,"MaxMll");}
   float minMll() {return cutReading(0,"MinMll");}
   float maxdrL1RecoCone() {return cutReading(3,"MaxDrL1RecoCone"); }

 private:
    std::vector<std::string> vars,cuts;   
};

CutReader::CutReader(TString file){
   std::ifstream infile(file); std::string line;
   while (std::getline(infile, line)){
     if (line.rfind("#", 0)==0) continue;
     vector<string> tokens;   
     boost::split(tokens,line,boost::is_any_of("=")); 
     if (tokens.size()<2) continue;
     vars.push_back(tokens[0]); cuts.push_back(tokens[1]);   
   }
}

CutReader::~CutReader(){}

float CutReader::cutReading(float def,std::string cut){
  if (vars.size()==0) return def;
   float value=def;
   for (int i=0; i<vars.size(); i++){
     if (vars[i]!=cut) continue;
     value=std::stof(cuts[i]);
   }
   return value;
}

