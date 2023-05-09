

#ifndef __EPAXOS_INSTANCE__
#define __EPAXOS_INSTANCE__

#include "comminclude.h"
#include "status.h"
#include "rescode.h"
namespace epaxos {


class InstanceID {
public:
    InstanceID():insid_(0){}
    InstanceID(const InstanceID & t):insid_(t.insid_+1){}
    bool IsNull(){return insid_==0;}
private:
    uint64_t insid_;    //自增号
    uint64_t seqid_;    //序列id，允许重复
    Status state_;      //状态
};

class InstanceArrIds {
public:
    InstanceArrIds(bool isLocal):bIsLocal_(isLocal){}
    bool IsLocalArr(){return bIsLocal_;}
private:
    uint32_t insId_;
    bool bIsLocal_;
    std::vector<InstanceID> arryins_;
};

class InstanceNode {
public:
    
private:
    uint32_t localInstId_;                  //本地的insid
    std::vector<InstanceArrIds*> inslist_;  //实例列表
};

}

#endif