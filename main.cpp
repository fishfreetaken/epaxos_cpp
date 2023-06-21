
#include <stdio.h>
#include <stdlib.h>


#include <cstdlib>
#include <ctime>

#include "imiate_test.h"
#include "iterallcase_test.h"
#include <set>

#include "util.h"

size_t uNodeNum = 5;


class IterAllCase{
public:
  IterAllCase(uint32_t baseNum,uint32_t caseNum):pos_(0),count_(baseNum),div_(caseNum){
    //获取全排列
    Init();
  }

  uint32_t GetOneCase(std::vector<uint32_t> &mt){
    if(pos_ >= vc_.size()){
      reset();
      return 0;
    }
    mt.clear();
    uint32_t t = vc_[pos_];
    uint32_t s=0;
    while(t>0){
      if( (t&1) >0 ){
        mt.push_back(s);
      }
      s++;
      t=t>>1;
    }
    return mt.size();
  }
  uint32_t GetAllCase(std::vector<std::vector<uint32_t >> &t){
      reset();
      std::vector<uint32_t> mt ;
      while(GetOneCase(mt)!=0){
        t.push_back(mt);
      }
      return t.size();
  }

  //把一个vector进行重新排列放入
  static void GetIterMaster(uint32_t ibp , std::vector<std::vector<uint32_t>>& outp){
      std::vector<uint32_t > rt;
      rt.resize(ibp);
      std::iota(rt.begin(),rt.end(),0);
      backtrack(outp,rt,0);
  }

private:
  void reset(){
    pos_=0;
  }

  static void backtrack(std::vector<std::vector<uint32_t>>& res, std::vector<uint32_t>& input, uint32_t first){
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
  void Init(){
    std::set<int> st;
    dfs(st,div_,0,0);
    for(auto it=st.begin();it!=st.end();it++){
      vc_.push_back(*it);
    }
    std::cout<<"last total count "<<vc_.size()<<std::endl;
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

void TestSingleCas(){
  epaxos_client::OperationKVArray kv;
  IterAllCase at(uNodeNum,uNodeNum/2+1);

  std::vector<uint32_t > mt;
  size_t incSeq = 1;
  while(at.GetOneCase(mt)){
    size_t i=0;
    uint32_t master = 0;
    std::vector<std::vector<uint32_t>> Permatutions;

    std::vector<uint32_t> rt;
    for(i=0;i<mt.size();i++){
      if(mt[i]>0){
        rt.push_back(i);
      }
    }
    at.GetIterMaster(rt.size(),Permatutions);
    for(size_t j=0;j<Permatutions.size();j++){
      std::stringstream stt;
      epaxos::InstanceSwap sp;
      for(i=0;i<Permatutions[j].size();i++){
        int pos = Permatutions[j][i];
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
      for(i=0;i<Permatutions[j].size();i++){
        size_t pos =Permatutions[j][i];
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
}

int main(){

  for(size_t i=0;i<uNodeNum;i++){
    gIns.push_back(new epaxos::InstanceNode(i,uNodeNum));
  }
  /*
  epaxos_test::Case_Node a(1,123);
  epaxos_test::Case_Node b(2);

  std::cout<<a.Value()<<b.Value()<<std::endl;
  std::swap(a,b);
  std::cout<<a.Value()<<b.Value()<<std::endl;
  return 1;
  */
  std::vector<epaxos_test::Case_Node> tmpl;
  tmpl.emplace_back(epaxos_test::Case_Node(1,4));
  tmpl.emplace_back(epaxos_test::Case_Node(2));
  tmpl.emplace_back(epaxos_test::Case_Node(3));
  tmpl.emplace_back(epaxos_test::Case_Node(4));
  tmpl.emplace_back(epaxos_test::Case_Node(5));
  tmpl.emplace_back(epaxos_test::Case_Node(6));
  //tmpl.emplace_back(epaxos_test::Case_Node(7));

  epaxos_test::Iter_all_cases<epaxos_test::Case_Node> mt(tmpl);

  //return 1;
  std::cout<<"init num "<< mt.Init() << std::endl;

  std::vector<epaxos_test::Case_Node> *st;
  size_t count=0;
  while( nullptr != (st=mt.GetOneCase())){
     std::cout<< PrintfVectorN<epaxos_test::Case_Node>(*st)<<std::endl;
     count++;
  }
  std::cout<<"total count "<<count << std::endl;

  return 0;
  /*
  epaxos::DepsIDs dps(5);
  dps.UpdateOne(1,2);
  dps.UpdateOne(4,23);

  epaxos_client::OperationKVArray kv;
  epaxos::Instance ins(dps,epaxos::MutexSeqID(2),kv);
  epaxos::InstanceSwap ep(1,2,&ins);

  std::cout << ep.GetDetailInfo() << std::endl; 

  //dps.UpdateOne(0,134);
  //epaxos::Instance ins2(dps,epaxos::MutexSeqID(7),kv);
  epaxos::InstanceSwap st  = ep.New();
  st.GetMutalInsPtr()->RefreshDeps(0,123);
  std::cout << st.GetDetailInfo() << std::endl;

  std::cout << ep.GetDetailInfo() << std::endl; 
  printf("addr1:%d addr2:%d addr3:%d\n",st.GetMutalInsPtr(),ep.GetMutalInsPtr(),&ins);
  return 0;
  */

  size_t subNum = uNodeNum/2 +1;
  IterAllCase at(uNodeNum,subNum); // 5 选3 

  size_t writerNum = 2;
  IterAllCase wrieterCase(uNodeNum,writerNum); //5 / 2

  //对case进行重排列
  std::vector<uint32_t> writerOneCases;
  while(wrieterCase.GetOneCase(writerOneCases)){ // 0 1; 0 2; 0 3; 0 4; 0 5;
      //选出一个writer
      epaxos_test::WorkManager wm(writerOneCases.size());
      size_t ciclyNum =  writerOneCases.size() -1;
      for(size_t iw = 0;iw < ciclyNum ;iw++){
        std::vector<uint32_t> allOneCases;  //所有
        at.GetOneCase(allOneCases);
        wm.Init(allOneCases,allOneCases); //前一个随机出来一个排列后再插入一个新的排列
      }

      //size_t st= wm.Init(writerOneCases,allOneCases);
      std::vector<std::vector<uint32_t>> Permatutions; //全排列
      IterAllCase::GetIterMaster(ciclyNum,Permatutions);
      std::cout<<" writercase: "<< PrintfVector(writerOneCases) << "allcase: "<<PrintfVector(writerOneCases)<< " Permatutions "<< PrintfVector(Permatutions)<<std::endl <<std::endl;
      break;
      /*
        size_t tk = Permatutions.size();
        std::cout<<"Permatutions size:"<< tk<<std::endl;
        tk = 4;
        for(size_t i =0;i<tk;i++){
          for(size_t j=0;j<Permatutions[i].size();j++){
            std::cout << i<< " "<< j << " Permatutions[i]:" << Permatutions[i][j]<< "||" << wm.StepAt(Permatutions[i][j]).GetDetailInfo()<<std::endl<< std::endl;
            //break;
          }
          //需要对提出的值进行
          wm.Reset();
        }
      */
      break;
      
      break;
  }
  for(size_t t=0;t<uNodeNum;t++){
    std::cout << "master :"<< t<< gIns[t]->DebugPrintInstanceNode() << std::endl;
  }

  std::cout << "over" <<std::endl;
  return 0;
}