
#include <numeric>
#include "instance_mng.h"

namespace  epaxos {

ResCode BatchGetKvValueArray::GenNewInsMaxSeqID(std::vector<std::string>& mp,epxos_instance_proto::EpInstID * insid){
    //获取最大值
    std::unordered_map<std::string , epxos_instance_proto::EpValueItem> tmpvalue;
    std::vector<std::string> nofind;
    for(auto iter = mp.begin();iter!=mp.end();iter++){
        epxos_instance_proto::EpValueItem value;
        bool exist=cache_->tryGet(*iter,value);
        if(!exist){
            spdlog::trace("BatchGetKvValueArray::GenNewInsMaxSeqID local cache miss key:{} nofind:{}",*iter,nofind.size());
            nofind.push_back(*iter);
        }else{
            tmpvalue[*iter]=value;
        }
    }

    spdlog::trace("BatchGetKvValueArray::GenNewInsMaxSeqID nofind:{} tmpvalue:{}",nofind.size(),tmpvalue.size());

    for_each(nofind.begin(),nofind.end(),[&](std::string & key){
        epxos_instance_proto::EpValueItem v;
        this->ReadOneFromDb(key,v);
        tmpvalue[key] = v;
    });

    //每个value更新到最新
    uint64_t tmp_seq=0;
    for(auto iter = tmpvalue.begin();iter!=tmpvalue.end();iter++){
        if(iter->second.iid().seqid() >= tmp_seq){
            spdlog::trace("BatchGetKvValueArray::GenNewInsMaxSeqID: tmp:{} cur:{}",tmp_seq,iter->second.iid().seqid());
            tmp_seq = iter->second.iid().seqid()+1;
        }
    }

    insid->set_seqid(tmp_seq);

    for(auto iter = tmpvalue.begin();iter!=tmpvalue.end();iter++){
        iter->second.mutable_iid()->set_seqid(tmp_seq);
    }
    spdlog::trace("BatchGetKvValueArray::GenNewInsMaxSeqID: ready to batchset  tmpseq:{} tmpvalue:{}",tmp_seq,tmpvalue.size());
    return this->BatchSet(tmpvalue);
}

std::string InstanceManager::GetInscKey(const epxos_instance_proto::EpInstance&a){
    std::string res;

    
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
    
    return ResCode::Success();
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



