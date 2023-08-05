#ifndef __EPAXOS_INSTANCE_NODE_H__
#define __EPAXOS_INSTANCE_NODE_H__

#include "proto/instance.pb.h"
#include "include/rescode.h"

#include "include/rescode.h"

namespace epaxos{

class Instance{
public:
    Instance(){}    //从db中直接解析，或者直接拿到id
    Instance(epxos_instance_proto::EpInstance & t):ins_(t){}

    ResCode InitInsFromString(std::string &body);

    ResCode step(epxos_instance_proto::InstanceSwapMsg *ins);

    ResCode step_ack(const epxos_instance_proto::InstanceSwapMsg & ins);

private:
    ResCode PreAccept();
    ResCode Accept();
    ResCode Commit();

    ResCode PreAccept_Ack();
    ResCode Accept_Ack();

private:
    bool CanCommit();

private:
    epxos_instance_proto::EpInstance ins_;
};

};
#endif