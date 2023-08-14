#ifndef __EPAXOS_INSTANCE_NODE_H__
#define __EPAXOS_INSTANCE_NODE_H__

#include "proto/instance.pb.h"
#include "include/rescode.h"
#include "instance_mng.h"
namespace epaxos{

class Instance{
public:
    Instance(InstanceManager *t):is_empty_(true),mng_(t){}    //从db中直接解析，或者直接拿到id

    Instance(epxos_instance_proto::EpInstance &i,InstanceManager *t):ins_(i),is_empty_(false),mng_(t){}

    /**
     * @brief 实际进行处理
     * 
     * @param ins 
     * @return true 
     * @return false 
     */
    bool step(epxos_instance_proto::InstanceSwapMsg &ins);

    ResCode step_ack(const epxos_instance_proto::InstanceSwapMsg & ins);

    bool IsEmpty(){return is_empty_;}

private:
    ResCode PreAccept();
    ResCode Accept();
    ResCode Commit();

    ResCode PreAccept_Ack();
    ResCode Accept_Ack();

private:
    bool CanCommit();

private:
    epxos_instance_proto::EpInstance ins_; //本地

    bool is_empty_;

    InstanceManager *mng_;

};

};
#endif