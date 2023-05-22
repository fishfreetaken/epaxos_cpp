
#include <stdio.h>
#include <stdlib.h>

#include "src/instance.h"

int uNodeNum = 5;
std::vector<epaxos::InstanceNode *> gIns;

void IterAllInfo(uint64_t generator ){

  for(int i=0;i<uNodeNum;i++){
    gIns.push_back(new epaxos::InstanceNode(i,uNodeNum));
  }

  /** 0 生成；1 插入
   * 
   */
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

int main(){
/*
  epaxos_client::OperationKVArray kv;
  for (int i=0;i<12;i++) {
    epaxos_client::OperationKV t(std::string("vapourchen"));
    kv.InsertOne(std::string("chendong"),t);
 
    epaxos::InstanceIDSwap tInsId  = gIns.at(i%uNodeNum)->GenLocalNewInstanceId(kv);

    //随机插入
    std::for_each(gIns.begin(),gIns.end(),[&tInsId](epaxos::InstanceNode *t ){
      t->InsertNewInst(tInsId);
    });
    std::cout << tInsId.DetailInfoAddNode() << std::endl;
  }
  */

  //生成
  //插入
  uint64_t allNum = 1<<uNodeNum;

  std::cout << "iter allnum: "<< allNum <<std::endl;

  for(size_t i = 0;i < allNum ;i++){
    IterAllInfo(i);
  }

  return 0;
}