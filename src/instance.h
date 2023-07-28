

#ifndef __EPAXOS_INSTANCE__
#define __EPAXOS_INSTANCE__

#include "../include/comminclude.h"
#include "status.h"

#include "../include/rescode.h"

#include "../include/storage_tranfer.h"

#include "instance.pb.h"

namespace epaxos {

class BatchGetKvValueArray : public StMemoryItemsArry {

public:
    BatchGetKvValueArray(){}

    ResCode ParseReq(const epxos_instance_proto::EpaxosInsWriteReq &pm_);

    ResCode BatchDecode(std::map<std::string,std::string> & mk);     //批量解码 

    ResCode BatchEncode(std::map<std::string,std::string> &mk)const; //批量加密

    //const
    void GetKeys(std::map<std::string,std::string> & mk)const;

    ResCode MergeGetNewIns(epxos_instance_proto::EpInstance*p); //融合生成一个新的ins

private:
    std::unordered_map<std::string , epxos_instance_proto::EpValueStorageBody> mep;
};

class InstanceNode{
public:
    InstanceNode(){}    //从db中直接解析，或者直接拿到id
    InstanceNode(epxos_instance_proto::EpInstance & a):ins_(a){}

    ResCode InitLocalIns(std::string &body);

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

class InstanceManager {
public:
    InstanceManager(epxos_instance_proto::EpGlobalConf *p); //本地的node

    //生成inst事件并保存本地
    ResCode GenNewInstance(const epxos_instance_proto::EpaxosInsWriteReq & arrvalues,
    epxos_instance_proto::InstanceSwapMsg &rspmsg);

    ResCode Step(epxos_instance_proto::InstanceSwapMsg & inswap);
private:
    static std::string GetInscKey(const epxos_instance_proto::EpInstance&a);
private:
    //本地存储，存放所有的内容
    StorageMsgTransfer<epxos_instance_proto::EpValueStorageBody> *db_kv_;   //专门用来存储dbd
    StorageMsgTransfer<epxos_instance_proto::EpInstance> *db_ins_; //专门用来存储

    epxos_instance_proto::EpGlobalConf *ccf_;
};

}
#endif