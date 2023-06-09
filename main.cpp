
#include <stdio.h>
#include <stdlib.h>

#include "src/instance.h"

#include <cstdlib>
#include <ctime>

#include <set>
size_t uNodeNum = 5;
std::vector<epaxos::InstanceNode *> gIns;

std::string PrintfVector(std::vector<uint32_t> & t){
  std::stringstream sst;

  for(size_t i=0;i<t.size();i++){
    sst<<t[i] <<" ";
  }
  return sst.str();
}

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

  uint32_t GetOneCase(std::vector<uint32_t> &mt){
    if(pos_ >= vc_.size()){
      return 0;
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
    return vc_[pos_++];
  }
  //把一个vector进行重新排列放入
  void GetIterMaster(std::vector<uint32_t> &ibp , std::vector<std::vector<uint32_t>>& outp){
      backtrack(outp,ibp,0);
  }
private:

  void backtrack(std::vector<std::vector<uint32_t>>& res, std::vector<uint32_t>& input, uint32_t first){
        // 所有数都填完了
        uint32_t len=  input.size();
        if (first == len) {
            res.emplace_back(input);
            return;
        }
        for (uint32_t i = first; i < len; ++i) {
            // 动态维护数组
            std::swap(input[i], input[first]);
            // 继续递归填下一个数
            backtrack(res, input, first + 1);
            // 撤销操作
            std::swap(input[i], input[first]);
        }
    }

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
  for(size_t i=0;i<uNodeNum;i++){
    gIns.push_back(new epaxos::InstanceNode(i,uNodeNum));
  }
  epaxos_client::OperationKVArray kv;

  //TestMutual2();
  //return 1;

  IterAllCase at(uNodeNum,3);
  at.Init();

  std::vector<uint32_t > mt;
  size_t incSeq = 1;
  while(at.GetOneCase(mt)){
    //std::cout << VectorTransferToString(mt)<<std::endl;

    size_t i=0;
    uint32_t master = 0;
    std::vector<std::vector<uint32_t>> allcase;

    std::vector<uint32_t> rt;
    for(i=0;i<mt.size();i++){
      if(mt[i]>0){
        rt.push_back(i);
      }
    }
    at.GetIterMaster(rt,allcase);
    /*
      std::stringstream stt;
      for (i=0;i< allcase.size();i++){
        stt << PrintfVector(mt) << "  case: " << PrintfVector(allcase[i]) << std::endl;;
      }
      std::cout << stt.str()<<std::endl;
      continue;
    */
    for(size_t j=0;j<allcase.size();j++){
      std::stringstream stt;
      epaxos::InstanceSwap sp;
      for(i=0;i<allcase[j].size();i++){
        int pos = allcase[j][i];
        if(sp.GetInsID().IsNull()){
          sp = gIns[pos]->GenNewInstance(kv);
          stt<<"[master:"<<pos<<"] ";
          master  = pos;
          std::cout<<"source sp"<< sp.GetDetailInfo() << std::endl;
        }else{
          epaxos::InstanceSwap tsp = sp.New();
          gIns[pos]->MutualManageIns(tsp);
          stt << pos <<" ";
          std::cout<<"tsp i: "<<pos<<" " <<tsp.GetDetailInfo()<<std::endl;
          gIns[master]->ReFreshLocal(tsp);
          std::cout << gIns[pos]->DebugPrintInstanceNode() << std::endl;
        }
      }
      epaxos::InstanceSwap nesp = gIns[master]->GetTargetIns(sp);
      for(i=0;i<allcase[j].size();i++){
        size_t pos =allcase[j][i];
        if(pos!=master){
          gIns[pos]->ReFreshLocal(nesp);
        }
      }
      std::cout << "master :"<<master << stt.str() << gIns[master]->DebugPrintInstanceNode() << std::endl<< std::endl;
      std::cout<<std::endl;

      assert(gIns[master]->GetSeq().Value64() == incSeq);
      incSeq++;
    }
  }

  //break;
  for(size_t i=0;i<uNodeNum;i++){
    std::cout << gIns[i]->GetAllCollectorInsInfo()<<std::endl;
    for(size_t j=0;j<uNodeNum;j++){
      if(i==j){
        continue;
      }
      epaxos::ResCode res = gIns[i]->Include(*gIns[j]);
      if (res.IsError()){
        std::cout<<"[Warning  node not include :" << res.GetRemote() << std::endl;
        assert(false);
      }
      
    }
  }
   
  return 0;
}