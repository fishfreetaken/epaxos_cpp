
#include <stdio.h>
#include <stdlib.h>

#include "src/instance.h"

int uNodeNum = 5;
std::vector<epaxos::InstanceNode *> gIns;
void FundGenOne(uint32_t nodeid, epaxos_client::OperationKVArray * t ){
    if(t==nullptr) {
      gIns[nodeid]->GenLocalNewInstanceId();
    }else{
      gIns[nodeid]->GenLocalNewInstanceId(*t);
    }
    
}

int main(){

  for(int i=0;i<uNodeNum;i++){
    gIns.push_back(new epaxos::InstanceNode(i,uNodeNum));
  }

  epaxos_client::OperationKVArray kv;
  for(int i=0;i<12;i++){
    epaxos_client::OperationKV t(std::string("vapourchen"));
    kv.InsertOne(std::string("chendong"),t);
    epaxos::InstanceIDSwap tInsId  = gIns.at(i%uNodeNum)->GenLocalNewInstanceId(kv);

    for(int j=0;j<uNodeNum;j++){
      if(j==i){
        continue;
      }
      gIns.at(j)->InsertNewInst(tInsId);
    }
    std::cout << tInsId.GetDetailInfo() << std::endl;
  }

  for(int i=0;i<uNodeNum;i++){
    
  }
  /*
    epaxos::InstanceIDSwap id2 =  gIns.at(1)->GenLocalNewInstanceId();

    gIns.at(0)->InsertNewInst(id2);
    std::cout<< gIns.at(0)->DebugPrintInstanceNode();
  */
  return 0;
 }