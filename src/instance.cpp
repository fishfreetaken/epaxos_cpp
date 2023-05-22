#include "instance.h"

#include <numeric>
namespace  epaxos {

void DepsIDs::Update(NodeID idx,const InsID &a){
    at(idx)->Update(a);
}

Instance::Instance(NodeSize s, const epaxos_client::OperationKVArray & arrvalues):deps_(s){
    arrvalues.GetArrKeys(arrkeys_);
}

/**
 * @brief 
 * 
 * @param ins 
 */
void Instance::Update(const NodeID nid, const MutexSeqID &seq , const InsID &ins ){

    seqid_.UpdateAhead(seq);

    deps_.Update(nid,ins);
}

bool Instance::Update(const Instance & a){
    if(state_.Behand(a.state_)){
        return false;
    }
    seqid_.Update(a.seqid_);

    deps_.Update(a.deps_);
    state_ = a.state_;
    return true;
}

std::string Instance::GetDetailInfo()const{
    std::stringstream st;
    st << " [SeqID: " << seqid_ << "] ";
    st << " [INSID: " << insid_ << "]" << " [DEPS: ";
    std::for_each(deps_.begin(),deps_.end(),[&](const  uint64_t &a ){
        st << a <<" ";
    });
    st << "] [KEYS: ";

    std::for_each(keys_.begin(),keys_.end(),[&](const std::string &a ){
        st << a;
    });
    st << "]";
    return st.str();
}


Instance* InstanceArrIds::GetMaxmumInstanID()const{
    std::map<uint64_t,Instance*>::const_reverse_iterator riter = arryins_.rbegin();

    if(riter == arryins_.rend()){
        return nullptr;
    }
    return riter->second;
}

std::string InstanceArrIds::GetArrayDetail()const{
    std::stringstream st;
    st << "[Node: " << nodeID_ <<"] ";
    st << "[CurMaxIns: " << curMaxInsId_ << "] "<< std::endl;
    std::for_each(arryins_.begin(),arryins_.end(),[&](const std::pair <uint64_t,Instance *>  & value){
        st << "      " << "[InsID: "<<value.first <<"] : " << value.second->GetDetailInfo() << std::endl;
    });
    return st.str();
}


uint64_t InstanceArrIds::IncLocalMaxInsID(){
    return  ++curMaxInsId_;
}

/**
 * @brief 获取最新的instance的信息对seq进行递增
 * 
 * @param nodesize 
 * @return Instance* 
 */
Instance* InstanceArrIds::GenNewInstanceID(uint32_t nodesize){
    //lock
    Instance *last = GetMaxmumInstanID();
    assert(last!=nullptr);

    uint64_t lMaxIns = IncLocalMaxInsID();

    //获取当前seq的最大值
    auto fMaxSeq = uint64_t [&last] (){ return last->GetSeqId()+1;}

    //获取上一个ins的seq和insid
    Instance *ins = new Instance(lMaxIns,nodeID_,nodesize,fMaxSeq() );
    assert(ins!=nullptr);
    //获取上一个ins的seq
    
    arryins_[curMaxInsId_]=ins;
    //unlock
    return ins;
}

ResCode InstanceArrIds::InserNewIns(const Instance & ins){
    auto iter = arryins_.find(ins.GetInstId());
    if(iter == arryins_.end()){
        //直接拷贝
        arryins_[ins.GetInstId()]= new Instance(ins);
    }else {
        iter->second->NewerAndUpdate(ins);
    }
    
    return ResCode(0);
}

InstanceNode::InstanceNode(uint32_t locNodeID, uint32_t nodeNum):
                            nodeSize_(nodeNum)

{
    assert(locNodeID <= nodeNum);
    inslist_.resize(nodeNum,1);
    
    std::iota(inslist_.begin(),inslist_.end(),0);

    localArray_ =  &inslist_[locNodeID];
    localArray_->SetLocalArr(true);
}

ResCode InstanceNode::InsertNewInst(const InstanceIDSwap & inswap){
    //自己不会插入自己的值
    if(localArray_->IsLoacalIns(inswap)){
        return ResCode(-1);
    }

    size_t indexNode = inswap.GetNodeID();

    //找到对应的队列
    assert(indexNode < inslist_.size() );
    return  inslist_.at(indexNode).InserNewIns(inswap);
}

/**
 * @brief value进行存储，key保存在ins中
 * 
 * @param keys 
 * @param values 
 * @return Instance* 
 */
InstanceIDSwap InstanceNode::GenLocalNewInstanceId(const epaxos_client::OperationKVArray & arrvalues){
    Instance*  p_ins = localArray_->GenNewInstanceID(nodeSize_);
    //写入db key values

    //更新deps
    std::for_each(inslist_.begin(),inslist_.end(),[&](const InstanceArrIds&ins){
        p_ins->UpdateSeqAndInsertDep(ins);
    });

    p_ins->InsertKeys(arrvalues);
    return InstanceIDSwap(localArray_->GetNodeID(),*p_ins);
}

InstanceIDSwap InstanceNode::GenLocalNewInstanceId(){
    epaxos_client::OperationKVArray kvarr;
    return GenLocalNewInstanceId(kvarr);
}

std::string InstanceNode::DebugPrintInstanceNode(){
    std::stringstream st;
    st<< "DebugPrintInstanceNode getdetail: " <<std::endl;
    std::for_each(inslist_.begin(),inslist_.end(),[&st](const InstanceArrIds &a ){
        st << a.GetArrayDetail() <<std::endl;
    });
    return st.str();
}

};