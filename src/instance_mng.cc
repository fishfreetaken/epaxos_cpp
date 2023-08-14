
#include <numeric>
#include "instance_mng.h"

namespace  epaxos {


ResCode BatchGetKvValueArray::Updatekeys(epxos_instance_proto::EpInstance &ins){
    std::vector<std::string> nofind;
    for(int i =0;i< ins.depsids().item_size();i++){
        const epxos_instance_proto::EpKeyValueItem&tmp = ins.depsids().item(i);
        epxos_instance_proto::EpKeyValueItem value;
        bool exist=cache_->tryGet(tmp.key(),value);
        if(!exist){
            nofind.push_back(tmp.key());
        }

    }
}

//
ResCode BatchGetKvValueArray::LoadkeysItems(epxos_instance_proto::EpInstance &ins,std::unordered_map<std::string , epxos_instance_proto::EpKeyValueItem> tmpvalue){
    ResCode r;
    std::vector<std::string> nofind;
    for(int i =0;i< ins.depsids().item_size();i++){
        const epxos_instance_proto::EpKeyValueItem&tmp = ins.depsids().item(i);
        epxos_instance_proto::EpKeyValueItem value;
        bool exist=cache_->tryGet(tmp.key(),value);
        if(!exist){
            nofind.push_back(tmp.key());
        }else{
            spdlog::trace("BatchGetKvValueArray::GenNewInsMaxSeqID local cache target key:{} nofind:{}",tmp.key(),tmpvalue.size());
            tmpvalue[tmp.key()] = value;
        }
    }
    spdlog::trace("BatchGetKvValueArray::GenNewInsMaxSeqID nofind:{} tmpvalue:{}",nofind.size(),tmpvalue.size());

    for_each(nofind.begin(),nofind.end(),[&](std::string & key){
        epxos_instance_proto::EpKeyValueItem v;
        r = this->ReadOneFromDb(key,v);
        if(r.IsError() && !r.IsNotFound()){
            spdlog::error("invalid key readfailed :{} ",key);
        }
        tmpvalue[key] = v;
    });
    return r;
}

ResCode BatchGetKvValueArray::GenNewInsMaxSeqID(epxos_instance_proto::EpInstance &ins){
    //获取最大值
    std::unordered_map<std::string , epxos_instance_proto::EpKeyValueItem> tmpvalue;
   
    LoadkeysItems(ins,tmpvalue);

    //每个value更新到最新，每个都要遍历一遍
    uint64_t tmp_seq=0;
    for(int i =0;i< ins.depsids().item_size();i++){
        epxos_instance_proto::EpKeyValueItem *tmp = ins.mutable_depsids()->mutable_item(i);
        auto iter = tmpvalue.find(tmp->key());
        assert(iter!=tmpvalue.end());
        tmp->mutable_iid()->CopyFrom(iter->second.iid());
        if(tmp->iid().seqid() >= tmp_seq ){
            tmp_seq = tmp->iid().seqid()+1;
        }
    }

    ins.mutable_iid()->set_seqid(tmp_seq);

    for(auto iter = tmpvalue.begin();iter!=tmpvalue.end();iter++){
        iter->second.mutable_iid()->CopyFrom(ins.iid());
    }

    spdlog::trace("BatchGetKvValueArray::GenNewInsMaxSeqID: ready to batchset  tmpseq:{} tmpvalue_size:{}",tmp_seq,tmpvalue.size());
    return this->BatchSet(tmpvalue);
}

ResCode BatchGetKvValueArray::GenNewInsMaxSeqID(epxos_instance_proto::EpInstance &ins){
    //批量拉取所有key和value
}

InstanceManager::InstanceManager(){
    plocalinstmng_ = new NodeLocalInstanceId();

    plocalinstmng_->Init();
    assert(plocalinstmng_ != nullptr);
    parr_ = new BatchGetKvValueArray(plocalinstmng_->GetPublicDb());
    assert(parr_ != nullptr);

    db_ = new ProtobufCacheHandler<epxos_instance_proto::EpInstance>(plocalinstmng_->GetInnerDb());
    assert(db_!=nullptr);
}

InstanceManager::~InstanceManager(){
    delete parr_;
    delete db_;
    delete plocalinstmng_; 
    parr_ = nullptr;
    db_ = nullptr; 
    plocalinstmng_ = nullptr;
}

/**
 * @brief value进行存储，key保存在ins中
 * 
 * @param keys 
 * @param values 
 * @return Instance* 
 */
ResCode InstanceManager::GenNewInstance(const epxos_instance_proto::EpaxosInsWriteReq & reqValue,epxos_instance_proto::InstanceSwapMsg &rspmsg){
    //获取最大的seqid

    //获取当前的instid，存在本地的文件中
    epxos_instance_proto::EpInstance *ins = rspmsg.mutable_insc();
    //本地的事件增加1

    for(auto iter=reqValue.list().begin();iter!=reqValue.list().end();iter++){
        epxos_instance_proto::EpKeyValueItem *tmpitem =  ins->mutable_depsids()->add_item();
        tmpitem->CopyFrom(*iter);
        tmpitem->mutable_iid()->Clear();
    }

    ins->mutable_iid()->set_insid(plocalinstmng_->IncId());

    ins->mutable_iid()->mutable_nodeid()->CopyFrom(plocalinstmng_->GetLocalCf().local_nodeid());

    ins->set_sate(epxos_instance_proto::EPXOS_EM_WK_PREACCEPT);


    //更新最大seq以及依赖
    ResCode r = parr_->GenNewInsMaxSeqID(*ins);

    //保存这个值，保存到cache 以及db
    r = SaveInstance(*ins);

    return r;
}

std::shared_ptr<Instance> InstanceManager::LoadInstance(const epxos_instance_proto::EpInstID&iid){
    std::string key;

    BuildInstanceKey(key,iid);

    epxos_instance_proto::EpInstance ins;
    //加一个队列？

    ResCode r= db_->Get(key, ins);

    if(r.IsError()&& !r.IsNotFound()){
        spdlog::error("local instance failed k:{} r: {}",key,r.GetRemote());
    }
    if(r.IsNotFound()){
        spdlog::debug("local instance not found key:{}",key);
    }
    return std::shared_ptr<Instance>(ins,this);
}

ResCode InstanceManager::SaveInstance(const epxos_instance_proto::EpInstance&ins){
    std::string key;
    BuildInstanceKey(key,ins.iid());
    return db_->Set(key,ins);
}

void InstanceManager::BuildInstanceKey(std::string &key,const epxos_instance_proto::EpInstID&ins){
        key.clear();
        std::stringstream st;
        st << "ins_" << ins.seqid()<<"_" << ins.insid()<< "_" << ins.nodeid().id();
        key = st.str();

        spdlog::trace("build ins key:{}",key);
}

/**
 * @brief 如果不存在就插入，如果存在就更新本地，如果本地超前，就更新对方
 * 
 * @param inswap 
 * @return ResCode 
 */
ResCode InstanceManager::Step(epxos_instance_proto::InstanceSwapMsg & inswap){
    ResCode res;
    //check from 
    if(inswap.to_node().id() != plocalinstmng_->GetLocalCf().local_nodeid().id()){
        spdlog::error("invalid inswap:{}",inswap.DebugString());
        return ResCode::InvalidParamErr();
    }

    //每一个swap的值
    auto pocalins = LoadInstance(inswap.insc().iid());

    bool sync = false;
    if(inswap.is_ack()){
        sync  = pocalins->step_ack(inswap);
    }else {
        sync  = pocalins->step(inswap);
    }

    if(sync){
        //同步instance
        //同步相应对deps
    }

    return res;
}

};



