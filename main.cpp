
#include <stdio.h>
#include <stdlib.h>

#include "src/instance.h"

#include <cstdlib>
#include <ctime>

#include <set>
int uNodeNum = 5;
std::vector<epaxos::InstanceNode *> gIns;
/*
void IterAllInfo(uint64_t generator ){

  for(int i=0;i<uNodeNum;i++){
    gIns.push_back(new epaxos::InstanceNode(i,uNodeNum));
  }

  epaxos_client::OperationKVArray kv;
  std::cout << "[generator:" << generator<< "]:" <<std::endl;
  size_t i=0;
  while (generator > 0 ) {
    if ( ( generator & 1 ) > 0 ){
      //命中
      epaxos_client::OperationKV t(std::string("chendong") + std::to_string(i));
      kv.InsertOne(std::to_string(generator) + "_" + std::to_string(i),t);
      epaxos::InstanceIDSwap tInsId = gIns.at(i%uNodeNum)->GenLocalNewInstanceId(kv);
      //遍历掺入
      std::cout << "  i:" << i << " "<< tInsId.DetailInfoAddNode() << std::endl;
    }
    i++;
    generator = generator>>1;
  }

  for(int i=0;i < uNodeNum;i++){
    delete gIns[i];
  }
  gIns.clear();
}
*/

class ModFive{
public:
  ModFive():val_(5){}
  uint32_t Value()const {  return val_;}
private:
  uint32_t val_;
};

template<typename T>
class CicyleMove{
public:
  CicyleMove():mv_(0){}
  CicyleMove(uint32_t t ):mod_(),mv_(t%mod_.Value()){}

  uint32_t Value(){return mv_;}
  uint32_t Step(const CicyleMove & t){
    Step();
    while(t.mv_ == mv_){
      Step();
    }
    return mv_;
  }

  uint32_t Step(){ mv_ = (mv_+1)%mod_.Value(); return mv_;}

private:
    T mod_;
    uint32_t mv_;
};


class IterAllCase{
public:
  IterAllCase(uint32_t baseNum,uint32_t caseNum):pos_(0),count_(baseNum),div_(caseNum){
    //获取全排列
  }

  void Init(){
    std::set<int> st;
    dfs(st,div_,0,0);
    for(auto it=st.begin();it!=st.end();it++){
      vc_.push_back(*it);
    }
    std::cout<<"last total count "<<vc_.size()<<std::endl;
  }

  void Reset(){
    pos_=0;
  }

  bool GetOneCase(std::vector<uint32_t> &mt){
    if(pos_ >= vc_.size()){
      return false;
    }
    mt.clear();
    mt.resize(count_,0);
    uint32_t t = vc_[pos_];
    //std::cout<< pos_<<" GetOneCase " << t<<std::endl;
    uint32_t s=0;
    while(t>0){
      if( (t&1) >0 ){
        mt[s]=1;
      }
      s++;
      t=t>>1;
    }
    pos_++;
    return true ;
  }

private:
  void dfs(std::set<int> &ans,uint32_t level,uint32_t begin, uint32_t val){
    //std::cout<<level<<" "<<begin<< " "<< val<<std::endl;
    if(level==0){
      ans.insert(val);
      return ; 
    }

    level--;
    for(size_t i=begin;i<count_;i++){
      val= val | (1<<i);
      dfs(ans,level,i+1,val);
      val = val & (~(1<<i));
    }
  }
private:
  std::vector<uint32_t> vc_;
  uint32_t pos_;
  uint32_t count_;
  uint32_t div_;
};

std::string VectorTransferToString(std::vector<uint32_t >&t){
    std::stringstream mt;
    for_each(t.begin(),t.end(),[&](uint32_t ss){
      mt << ss <<" ";
    });
    return mt.str();
}

void TestMutual1(){
  epaxos_client::OperationKVArray kv;

  epaxos::DepsIDs dp(uNodeNum);

  dp.UpdateOne(1,1);
  dp.UpdateOne(4,1);

  epaxos::Instance *ins=  new epaxos::Instance(dp,epaxos::MutexSeqID(1),kv);

  epaxos::InstanceSwap sp(epaxos::NodeID(1),epaxos::InsID(1),ins);

  gIns[0]->MutualManageIns(sp);

  std::cout<< sp.GetDetailInfo() << std::endl;

  epaxos::InstanceSwap swp = gIns[0]->GenNewInstance(kv);

  swp = gIns[0]->GenNewInstance(kv);

  std::cout<< swp.GetDetailInfo() << std::endl;

  gIns[2]->MutualManageIns(swp);

  swp = gIns[2]->GenNewInstance(kv);

  std::cout<< swp.GetDetailInfo() << std::endl;
}

void TestMutual2(){
    epaxos_client::OperationKVArray kv;
  epaxos::InstanceSwap sp = gIns[0]->GenNewInstance(kv);
  sp = gIns[0]->GenNewInstance(kv);

  gIns[1]->MutualManageIns(sp);

  sp = gIns[0]->GenNewInstance(kv);

  gIns[1]->MutualManageIns(sp);

  sp = gIns[1]->GenNewInstance(kv);

  std::cout<< sp.GetDetailInfo() << std::endl;
}

int main(){
  for(int i=0;i<uNodeNum;i++){
    gIns.push_back(new epaxos::InstanceNode(i,uNodeNum));
  }
  epaxos_client::OperationKVArray kv;

  /*
    epaxos::InstanceSwap sp = gIns[0]->GenNewInstance(kv);
    epaxos::InstanceSwap sp1 = sp;

    epaxos::InstanceSwap tsp = sp1.New();
    std::cout<<sp.GetInsPtr()->DebugInfo() <<std::endl;
    std::cout<<tsp.GetInsPtr()->DebugInfo() <<std::endl;
    printf("addr:%lld addr %lld %lld \n",(unsigned long long )sp.GetInsPtr(),(unsigned long long )sp1.GetInsPtr(),(unsigned long long )tsp.GetInsPtr());

    return 1;
  */

  //TestMutual2();
  //return 1;

  IterAllCase at(5,3);
  at.Init();

  CicyleMove<ModFive> p1;
  CicyleMove<ModFive> p2;

  CicyleMove<ModFive> p;

  std::vector<uint32_t > mt;
  while(at.GetOneCase(mt)){
    //std::cout << VectorTransferToString(mt)<<std::endl;
    size_t i=0;
    epaxos::InstanceSwap sp;

    uint32_t master = 0;
    std::stringstream stt;
    for(;i<mt.size();i++){
      if(mt[i]>0){
        if(sp.GetInsID().IsNull()){
          sp = gIns[i]->GenNewInstance(kv);
          stt<<"[master:"<<i<<"] ";
          master  = i;
        }else{
          epaxos::InstanceSwap tsp = sp.New();
          gIns[i]->MutualManageIns(tsp);
          stt << i <<" ";
          std::cout<<"tsp i: "<<i<<" " <<tsp.GetDetailInfo()<<std::endl;
          gIns[master]->ReFreshLocal(tsp);
          std::cout << gIns[i]->DebugPrintInstanceNode() << std::endl;
        }
      }
    }
  
    //master 反过来要更新本地的这个值
    //gIns[master]->MutualManageIns(sp);
    std::cout << "master :"<<master << stt.str() << gIns[master]->DebugPrintInstanceNode() << std::endl<< std::endl;
    //std::cout<< stt.str()<< " [pos:"<<i <<"] " << sp.GetDetailInfo()<< std::endl;
  }
  return 0;
}