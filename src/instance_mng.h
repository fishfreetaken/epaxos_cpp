

#ifndef __EPAXOS_INSTANCE__
#define __EPAXOS_INSTANCE__

#include "include/comminclude.h"

#include "include/rescode.h"

#include "proto/instance.pb.h"

#include "leveldb_kv.h"
#include "node_num_cfg.h"
#include <memory>
#include "instance_node.h"
namespace epaxos {
class BatchGetKvValueArray:public ProtobufCacheHandler<epxos_instance_proto::EpKeyValueItem>{
public:
    BatchGetKvValueArray(StorageBaseInterface * db):ProtobufCacheHandler<epxos_instance_proto::EpKeyValueItem>(db){
        assert(db!=nullptr);
    }

    ResCode LoadkeysItems(epxos_instance_proto::EpInstance &ins,std::unordered_map<std::string , epxos_instance_proto::EpKeyValueItem> tmpvalue);

    //生成一个新的事件，获取最大seq
    ResCode GenNewInsMaxSeqID(epxos_instance_proto::EpInstance & insid);

    //拉取deps对应的所有的key以及key的值
    ResCode Updatekeys(std::unordered_map<std::string , epxos_instance_proto::EpKeyValueItem> tmpvalue);
};

class InstanceManager {
public:
    friend Instance;
    InstanceManager(); //本地的node
    ~InstanceManager();

    //生成inst事件并保存本地
    ResCode GenNewInstance(const epxos_instance_proto::EpaxosInsWriteReq & arrvalues,
    epxos_instance_proto::InstanceSwapMsg &rspmsg);

    ResCode Step(epxos_instance_proto::InstanceSwapMsg & inswap);

private:
    ResCode SaveInstance(const epxos_instance_proto::EpInstance&ins);

    std::shared_ptr<Instance> LoadInstance(const epxos_instance_proto::EpInstID&iid);     //从存储里加载出instance

    void BuildInstanceKey(std::string &key,const epxos_instance_proto::EpInstID&ins);

private:
    NodeLocalInstanceId  *plocalinstmng_;

    BatchGetKvValueArray *parr_;

    ProtobufCacheHandler<epxos_instance_proto::EpInstance> * db_;
};

}

#endif