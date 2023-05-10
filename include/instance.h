

#ifndef __EPAXOS_INSTANCE__
#define __EPAXOS_INSTANCE__

#include "comminclude.h"
#include "status.h"
#include "rescode.h"
namespace epaxos {

class SeqId{
public:
    SeqId();
private:
    uint64_t seq_;
    uint32_t nodeId_;  //提出的节点id
};

class InstanceID {
public:
    InstanceID():insid_(0){}
    InstanceID(const InstanceID & t):insid_(t.insid_+1){}
    bool IsNull(){return insid_==0;}
private:
    uint64_t insid_;        //事件id
    SeqId seqid_;           //全局唯一
    WorkStatus state_;      //状态
    std::string key_;       //根据key索引value
};

class InstanceArrIds {
public:
    InstanceArrIds(bool isLocal):bIsLocal_(isLocal){}
    bool IsLocalArr(){return bIsLocal_;}
private:
    uint32_t insId_;
    bool bIsLocal_;
    std::vector<InstanceID> arryins_; //双端队列
};

class InstanceNode {
public:
    
private:
    uint32_t localInstId_;                  //本地的insid
    std::vector<InstanceArrIds*> inslist_;  //实例列表
};

}

#endif