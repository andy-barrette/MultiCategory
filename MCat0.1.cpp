#include<vector>
#include<cmath>
#include<fstream>
#include<iostream>
#include<string>
using namespace std;

string itos(int n){
    int i;
    string s;
    s.clear();
    for(i=8;i>=0;i--)if(n>=pow(10,i))break;
    for(;i>=0;i--){
        if(n>=pow(10,i)){
            s.push_back(floor(n/pow(10,i))+48);
            n=fmod(n,pow(10,i));
        }
        else s.push_back(48);
    }
    return s;
}

class ruletype{
    public:
    int morphi,outputi;
    vector<int> inputi;
    ruletype(){
        morphi=-1;
        outputi=-1;
    }
    bool isvalid(){
        if(morphi==-1)return false;
        if(outputi==-1)return false;
        if(inputi.size()==0)return false;
        return true;
    }
};

class cattype{
    vector<string> obj;
    vector<string> morph;
    vector<int> arity;
    vector<bool> commutative,invertible;
    vector<ruletype> rule;
    
    public:
        int objsize(){
            return obj.size();
        }
        int findobj(string n){
            int i;
            for(i=0;i<obj.size();i++)if(n==obj[i])return i;
            return -1;
        }
        int findmorph(string n){
            int i;
            for(i=0;i<morph.size();i++)if(n==morph[i])return i;
            return -1;
        }
        int applyrule(int rulenum,vector<int> input){ //Find rules that apply to list of inputs
            int i,r;
            bool valid=true;
            for(i=0;i<rule[rulenum].inputi.size();i++){
                if(rule[rulenum].inputi[i]!=input[i]){
                    valid=false;
                    break;
                }
            }
            if(valid)return rule[rulenum].outputi;

            if(commutative[rule[rulenum].morphi]){
                valid=true;
                for(i=1;i<=rule[rulenum].inputi.size();i++){
                    if(rule[rulenum].inputi[i-1]!=input[rule[rulenum].inputi.size()-i]){
                        valid=false;
                        break;
                    }
                }
            }
            if(valid)return rule[rulenum].outputi;
            
            if(invertible[rule[rulenum].morphi] && rule[rulenum].inputi.size()==1){
                if(rule[rulenum].outputi==input[0])return rule[rulenum].inputi[0];
            }
            if(valid)return rule[rulenum].outputi;
            
            return -1;
        }            
        int addobj(string n){
            if(n.size()==0)return 0;
            int start=0,end=0;
            while(start<n.size()){
                end=n.find(',',start);
                if(end==n.npos)end=n.size();
                if(findobj(n.substr(start,end-start))==-1)obj.push_back(n.substr(start,end-start));
                start=end+1;
            }
            return 1;
        }
        void addobj(string n,int i){
            obj[i]=n;
        }
        int addmorph(string n){
            if(n.size()==0)return 0;
            int start=0,end=0;
            while(start<n.size()){
                end=n.find(',',start);
                if(end==n.npos)end=n.size();
                if(findmorph(n.substr(start,end-start))==-1)morph.push_back(n.substr(start,end-start));
                arity.push_back((int)n[end+1]-48);
                if(morph.back().size()>1 && morph.back().back()=='c'){
                    morph.back().pop_back();
                    commutative.push_back(true);
                }
                else commutative.push_back(false);
                if(morph.back().size()>1 && morph.back().back()=='i'){
                    morph.back().pop_back();
                    invertible.push_back(true);
                }
                else invertible.push_back(false);
                start=end+3;
            }
            return 1;
        }
        int addrule(string n){
            if(n.size()==0)return 0;
            ruletype temp;
            int start=0,end=0;
            end=n.find(',',start);
            temp.morphi=findmorph(n.substr(start,end-start));
            start=end+1;
            while(n[end]!='=' && start<n.size()){
                end=n.find(',',start);
                if(end==n.npos)end=n.find('=',start);
                temp.inputi.push_back(findobj(n.substr(start,end-start)));
                start=end+1;
            }
            end=n.size();
            temp.outputi=findobj(n.substr(start,end-start));
            if(temp.isvalid()){
                rule.push_back(temp);
                return 1;
            }
            else return 0;
        }
        int download(char *filename){
            ifstream ifile(filename);
            ifile.seekg(0,ifile.beg);
            string line;
            getline(ifile,line);
            if(!addobj(line)){ifile.close();return 0;}
            getline(ifile,line);
            if(!addmorph(line)){ifile.close();return 0;}
            getline(ifile,line);
            while(line.size()>0){
                if(line[0]=='$' || line[0]=='\n'){
                    getline(ifile,line);
                    continue;
                }
                if(!addrule(line)){ifile.close();return 0;}
                if(ifile.tellg()==-1)break;
                getline(ifile,line);
            }
            ifile.close();
            return 1;
        }
        cattype* copy(cattype *c){
            int i;
            for(i=0;i<obj.size();i++)c->obj.push_back(obj[i]);
            for(i=0;i<morph.size();i++)c->morph.push_back(morph[i]);
            for(i=0;i<morph.size();i++)c->arity.push_back(arity[i]);
            for(i=0;i<morph.size();i++)c->commutative.push_back(commutative[i]);
            for(i=0;i<morph.size();i++)c->invertible.push_back(invertible[i]);
            for(i=0;i<rule.size();i++)c->rule.push_back(rule[i]);
            return c;
        }
        cattype* automap(char* filename,int itnum,cattype* c){ //Find c given suggested partial mapping of objects in filename. Use itnum rule iterations
            ifstream ifile(filename);
            ifile.seekg(0,ifile.beg);
            string line;
            copy(c);
            vector<int> mapping(obj.size(),-1);
            
            //Apply suggested partial mapping given in filename
            getline(ifile,line);
            while(line.size()>0){
                int arrow=line.find('>',0);
                if(findobj(line.substr(0,arrow))!=-1 && findobj(line.substr(arrow+1,line.size()-1-arrow))!=-1){
                    mapping[findobj(line.substr(0,arrow))]=findobj(line.substr(arrow+1,line.size()-1-arrow));
                }
                if(ifile.tellg()==-1)break;
                getline(ifile,line);
            }
            
            //Recursively apply rules in order to complete the mapping.
            int i,r,r0,in,result;
            bool all;
            vector<int> input;
            for(i=0;i<itnum;i++){
                for(r=0;r<rule.size();r++){
                    all=true;
                    input.clear();
                    for(in=0;in<rule[r].inputi.size();in++){
                        input.push_back(mapping[rule[r].inputi[in]]);
                        if(input.back()==-1){
                            all=false;
                            break;
                        }
                    }
                    if(all){
                        for(r0=0;r0<rule.size();r0++){
                            if(r==r0 || rule[r].morphi!=rule[r0].morphi)continue;
                            result=applyrule(r0,input);
                            if(result>=0){
                                mapping[rule[r].outputi]=result;
                            }
                        }
                    }
                }
            }
            ifile.close();
            ofstream ofile(filename,ios::trunc);
            string s;
            int counter=1;
            for(i=0;i<obj.size();i++){
                if(mapping[i]==-1){
                    c->obj[i]=itos(counter);
                    c->obj[i].insert(0,"a");
                    counter++;
                }
                else c->obj[i]=obj[mapping[i]];
                if(i>0)ofile<<"\n";
                ofile<<obj[i]<<">"<<c->obj[i];
            }
            ofile.close();
            return c;
        }
        void exportcat(char* filename){
            ofstream ofile(filename,ios::trunc);
            ofile.seekp(0);
            int i,in;
            ofile<<obj[0];
            for(i=1;i<obj.size();i++)ofile<<","<<obj[i];
            
            ofile<<"\n"<<morph[0];
            if(invertible[0])ofile<<"i";
            if(commutative[0])ofile<<"c";
            ofile<<","<<arity[0];
            for(i=1;i<morph.size();i++){
                ofile<<";"<<morph[i];
                if(invertible[i])ofile<<"i";
                if(commutative[i])ofile<<"c";
                ofile<<","<<arity[i];
            }
            
            ofile<<"\n"<<morph[rule[0].morphi];
            for(in=0;in<rule[0].inputi.size();in++)ofile<<","<<obj[rule[0].inputi[in]];
            ofile<<"="<<obj[rule[0].outputi];
            for(i=1;i<rule.size();i++){
                ofile<<"\n"<<morph[rule[i].morphi];
                for(in=0;in<rule[i].inputi.size();in++)ofile<<","<<obj[rule[i].inputi[in]];
                ofile<<"="<<obj[rule[i].outputi];
            }
            ofile.close();
        }
};

int main(){
    char ifilename[]="MC.txt";
    char mfilename[]="MCmap.txt";
    //char ofilename[]="MC--mapResult.txt";
    cattype cat,cat2;
    if(!cat.download(ifilename))cout<<"\nDownload Failed!\n";
    cat.automap(mfilename,10,&cat2);
    //cat2.exportcat(ofilename);
    return 1;
}
