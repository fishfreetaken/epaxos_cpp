

#ifndef __EPAXOS_INSTANCE__
#define __EPAXOS_INSTANCE__

#include "include/comminclude.h"

#include "include/rescode.h"

#include "proto/instance.pb.h"

#include "leveldb_kv.h"

namespace epaxos {

class BatchGetKvValueArray:public ProtobufCacheHandler<epxos_instance_proto::EpValueItem>{
public:
    BatchGetKvValueArray(StorageBaseInterface * db):ProtobufCacheHandler<epxos_instance_proto::EpValueItem>(db){
        assert(db!=nullptr);
    }

    //生成一个新的事件，获取最大seq
    ResCode GenNewInsMaxSeqID(std::vector<std::string>& mp, epxos_instance_proto::EpInstID * insid);
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

    epxos_instance_proto::EpGlobalConf *ccf_;
};

}

#endif