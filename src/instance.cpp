#include "instance.h"

#include <numeric>

namespace  epaxos {

//融合生成一个新的ins
ResCode BatchGetKvValueArray::MergeGetNewIns(epxos_instance_proto::EpInstance*p){
    ResCode res;
    assert(p!=nullptr);
    std::unordered_map<std::string , epxos_instance_proto::EpValueStorageBody>::iterator iter = mep.begin();
    for(;iter!=mep.end();iter++){
    }
    return res;
}

std::string InstanceManager::GetInscKey(const epxos_instance_proto::EpInstance&a){
    std::string res;
    a.iid().SerializeToString(&res);
    return res;
}

/**
 * @brief value进行存储，key保存在ins中
 * 
 * @param keys 
 * @param values 
 * @return Instance* 
 */
ResCode InstanceManager::GenNewInstance(const epxos_instance_proto::EpaxosInsWriteReq & reqValue,epxos_instance_proto::InstanceSwapMsg &rspmsg){
    //从db里加载这里的key-value对应的instance值
    BatchGetKvValueArray mt;
    mt.ParseReq(reqValue);

    ResCode res = db_kv_->BatchRead(&mt);
    
    mt.MergeGetNewIns(rspmsg.mutable_insc());

    std::string key= InstanceManager::GetInscKey(rspmsg.insc());
    //先将这个ins落地到当前存储中
    res = db_ins_->write(key,rspmsg.insc());

    //每个key还需要更新到最新的的ins

    epxos_instance_proto::EpNodeId *ptoNode = rspmsg.mutable_to_node();
    epxos_instance_proto::EpNodeId *pFromNode = rspmsg.mutable_to_node();

    pFromNode->set_id(ccf_->local_nodeid().id());
    return res;
}

/**
 * @brief 如果不存在就插入，如果存在就更新本地，如果本地超前，就更新对方
 * 
 * @param inswap 
 * @return ResCode 
 */
ResCode InstanceManager::Step(epxos_instance_proto::InstanceSwapMsg & inswap){
    epaxos::ResCode res;
    return res;
}

};



