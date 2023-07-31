#ifndef __EPAXOS_INSTANCE_NODE_H__
#define __EPAXOS_INSTANCE_NODE_H__

#include "proto/instance.pb.h"
#include "include/"

namespace epaxos{

class Instance{
public:
    Instance(){}    //从db中直接解析，或者直接拿到id

    ResCode InitInsFromString(std::string &body);

    ResCode step(epxos_instance_proto::EpInstance *ins);

    ResCode step_ack(const epxos_instance_proto::EpInstance & ins);

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