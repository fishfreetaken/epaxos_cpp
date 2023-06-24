
#include <stdio.h>
#include <stdlib.h>


#include <cstdlib>
#include <ctime>

#include "imiate_test.h"

size_t uNodeNum = 5;

void TestMutual1(){
  epaxos_client::OperationKVArray kv;

  epaxos::DepsIDs dp(uNodeNum);

  dp.UpdateOne(1,1);
  dp.UpdateOne(4,1);

  epaxos::Instance *ins=  new epaxos::Instance(dp,epaxos::MutexSeqID(1),kv);

  epaxos::InstanceSwap sp(epaxos::NodeID(1),epaxos::InsID(1),ins);

  gIns[0]->PreAccept(sp);

  std::cout<< sp.GetDetailInfo() << std::endl;

  epaxos::InstanceSwap swp = gIns[0]->GenNewInstance(kv);

  swp = gIns[0]->GenNewInstance(kv);

  std::cout<< swp.GetDetailInfo() << std::endl;

  gIns[2]->PreAccept(swp);

  swp = gIns[2]->GenNewInstance(kv);

  std::cout<< swp.GetDetailInfo() << std::endl;
}

void TestMutual2(){
    epaxos_client::OperationKVArray kv;
  epaxos::InstanceSwap sp = gIns[0]->GenNewInstance(kv);
  sp = gIns[0]->GenNewInstance(kv);

  gIns[1]->PreAccept(sp);

  sp = gIns[0]->GenNewInstance(kv);

  gIns[1]->PreAccept(sp);

  sp = gIns[1]->GenNewInstance(kv);

  std::cout<< sp.GetDetailInfo() << std::endl;
}

void TestSingleCas() {
  epaxos_client::OperationKVArray kv;
  epaxos_test::IterAllCase at(uNodeNum,uNodeNum/2+1);

  std::vector<uint32_t > mt;
  size_t incSeq = 1;
  while(at.GetOneCase(mt)){
    size_t i=0;
    uint32_t master = 0;
    

    std::vector<uint32_t> rt;
    for(i=0;i<mt.size();i++){
      if(mt[i]>0){
        rt.push_back(i);
      }
    }

    epaxos_test::Iter_all_cases<uint32_t> iterand(rt.size());
    iterand.InitAll();
  
    std::vector<uint32_t> *Permatutions;
    while(nullptr != (Permatutions = iterand.GetOneCase())){
      std::stringstream stt;
      epaxos::InstanceSwap sp;
      for(i=0;i<Permatutions->size();i++){
        int pos = Permatutions->at(i);
        if(sp.GetInsID().IsNull()){
          sp = gIns[pos]->GenNewInstance(kv);
          stt<<"[master:"<<pos<<"] ";
          master  = pos;
          std::cout<<"source sp"<< sp.GetDetailInfo() << std::endl;
        }else{
          epaxos::InstanceSwap tsp = sp.New();
          gIns[pos]->PreAccept(tsp);
          stt << pos <<" ";
          std::cout<<"tsp i: "<<pos<<" " <<tsp.GetDetailInfo()<<std::endl;
          gIns[master]->ReFreshLocal(tsp);
          std::cout << gIns[pos]->DebugPrintInstanceNode() << std::endl;
        }
      }
      epaxos::InstanceSwap nesp = gIns[master]->GetTargetIns(sp);
      for(i=0;i<Permatutions->size();i++){
        size_t pos =Permatutions->at(i);
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

//#define TEST_CASH_NODE 1
#define TEST_INSTANCE_SWAP 2
int main(){

  for(size_t i=0;i<uNodeNum;i++){
    gIns.push_back(new epaxos::InstanceNode(i,uNodeNum));
  }

  #ifdef TEST_CASH_NODE
    std::vector<epaxos_test::Case_Node> tmpl;
    tmpl.emplace_back(epaxos_test::Case_Node(0,1));
    tmpl.emplace_back(epaxos_test::Case_Node(1,5));
    tmpl.emplace_back(epaxos_test::Case_Node(2));
    tmpl.emplace_back(epaxos_test::Case_Node(3));
    tmpl.emplace_back(epaxos_test::Case_Node(4));
    tmpl.emplace_back(epaxos_test::Case_Node(5));
    tmpl.emplace_back(epaxos_test::Case_Node(6));
    tmpl.emplace_back(epaxos_test::Case_Node(7));

    // int t = 4;
    // std::cout<< t<<" : "<<tmpl[3].IsSegtive(tmpl[2]) << std::endl;
    // return 1;

    epaxos_test::Iter_all_cases<epaxos_test::Case_Node> mt(tmpl);

    //return 1;
    std::cout<<"init num "<< mt.InitSeg() << std::endl;

    std::vector<epaxos_test::Case_Node> *st;
    size_t count=0;
    while( nullptr != (st=mt.GetOneCase())){
      std::cout<< PrintfVectorN<epaxos_test::Case_Node>(*st)<<std::endl;
      count++;
    }
    std::cout<<"total count "<<count << std::endl;

    return 0;
  #endif

  #ifdef TEST_INSTANCE_SWAP

      epaxos_client::OperationKVArray kv;
      /*
        epaxos::DepsIDs dps(5);
        dps.UpdateOne(1,12);
        dps.UpdateOne(4,23);
      
        epaxos::Instance ins(dps,epaxos::MutexSeqID(2),kv);

        epaxos::InstanceSwap ep(1,2,&ins);
        
        dps.UpdateOne(0,98);
      
        std::cout << dps.DebugInfo() << std::endl;
        std::cout << ep.GetDetailInfo() << std::endl; 

        ep.UpdateDepsIns(4,14523);
        ep.GetMutalInsPtr()->RefreshDeps(1,123);
        ep.GetMutalInsPtr()->RefreshDeps(0,546);
        std::cout << ep.GetDetailInfo() << std::endl;

        //dps.UpdateOne(0,134);
        //epaxos::Instance ins2(dps,epaxos::MutexSeqID(7),kv);
        //epaxos::InstanceSwap st  = ep.New();
        //st.GetMutalInsPtr()->RefreshDeps(0,123);
        return 1;
      */

      epaxos::InstanceSwap *sp0 = new epaxos::InstanceSwap(gIns[0]->GenNewInstance(kv));
      epaxos::InstanceSwap *sp1 = new epaxos::InstanceSwap(gIns[1]->GenNewInstance(kv));
      epaxos::InstanceSwap tsp0 = sp0->New();
      epaxos::InstanceSwap tsp1 = sp1->New();
      gIns[1]->PreAccept(tsp0);
      gIns[0]->ReFreshLocal(tsp0); //这个更新本身

      std::cout << tsp0.GetDetailInfo() << " | " << gIns[0]->DebugPrintInstanceNode() << " || " << gIns[1]->DebugPrintInstanceNode()<<std::endl;

      gIns[0]->PreAccept(tsp1);
      gIns[1]->ReFreshLocal(tsp1); //这个更新本身
      std::cout << tsp1.GetDetailInfo() << " | " << gIns[0]->DebugPrintInstanceNode() << " || " << gIns[1]->DebugPrintInstanceNode()<<std::endl;

      epaxos::InstanceSwap tf1(0,1,nullptr);
      epaxos::InstanceSwap tf2(1,1,nullptr);

      std::cout<< gIns[0]->GetTargetIns(tf1).GetDetailInfo() << " || "<< gIns[1]->GetTargetIns(tf1).GetDetailInfo() <<std::endl;

      std::cout<< gIns[0]->GetTargetIns(tf2).GetDetailInfo() << " || "<< gIns[1]->GetTargetIns(tf2).GetDetailInfo() <<std::endl;

    return 1;
  #endif

  size_t subNum = uNodeNum/2 +1;
 
  epaxos_test::IterAllCase at(uNodeNum,subNum); // 5 选 3 
  size_t writerNum = 2;
  epaxos_test::IterAllCase wrieterCase(uNodeNum,writerNum); //5 / 2

  //对case进行重排列
  std::vector<uint32_t> writerOneCases;
  while(wrieterCase.GetOneCase(writerOneCases)){ // 0 1; 0 2; 0 3; 0 4; 0 5;
      //选出writer
      std::cout<< "writer:" << PrintfVector(writerOneCases)<<std::endl;
      epaxos_test::WorkManager wm(writerOneCases);
      at.reset();
      while(1){
        std::vector<std::vector<uint32_t>> receiverOneCase;
        for(size_t i=0;i<writerOneCases.size();i++) { //10 个
          std::vector<uint32_t> tmp;
          if(at.GetOneCase(tmp)==0){
            break;
          }
          receiverOneCase.push_back(tmp);
        }
        if (receiverOneCase.size()==0){
          std::cout<<"recive One Case null"<<std::endl;
          break;
        }
        std::cout<< "reciever:" << PrintfVector(receiverOneCase)<<std::endl;
        
        std::vector<epaxos_test::Case_Node> tmpl;
        wm.Init(receiverOneCase ,tmpl); //前一个随机出来一个排列后再插入一个新的排列
        
        epaxos_test::Iter_all_cases<epaxos_test::Case_Node> mt(tmpl);
        size_t segNum = mt.InitSeg();
        std::cout << "init seg num:"<<  segNum << " workmanager:" << wm.GetDetail() << std::endl;
        std::vector<epaxos_test::Case_Node> *st;
        size_t count=0;
        while( nullptr != (st=mt.GetOneCase())){
          std::cout << PrintfVectorN<epaxos_test::Case_Node>(*st)<<"detail swap:"<<std::endl;
          std::for_each(st->begin(),st->end(),[&wm](const epaxos_test::Case_Node & pos){
              epaxos::InstanceSwap sp =  wm.StepAt(pos.Value());
              std::cout<< sp.GetDetailInfo()<<std::endl;
          });
          count++;
          std::cout << std::endl;
          if(count < 2){
            break;
          }
        }
        break;
      }
      std::cout <<"==========================================="<<std::endl;
      break;
  }
  for(size_t t=0;t<uNodeNum;t++){
    std::cout << "master :"<< t<< gIns[t]->DebugPrintInstanceNode() << std::endl;
  }

  std::cout << "over" <<std::endl;
  return 0;
}