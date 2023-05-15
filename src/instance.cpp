
#include "instance.h"

#include <numeric>
namespace  epaxos {

InstanceID::InstanceID(uint64_t insId,uint32_t nodeid,uint32_t insNodeNum,uint64_t seq):insid_(insId),seqid_(seq){
    deps_.resize(insNodeNum,0);
    deps_[nodeid] = insId-1;
}

/**
 * @brief 
 * 
 * @param ins 
 */
void InstanceID::UpdateSeqAndInsertDep(uint32_t nodeid,const InstanceID *ins){

    if (seqid_ <= ins->seqid_){
        seqid_ = ins->seqid_ + 1;
    }
    assert(nodeid< deps_.size());
    deps_[nodeid] = ins->insid_;
}

void InstanceID::UpdateSeqAndInsertDep(const InstanceArrIds &arr){
    UpdateSeqAndInsertDep(arr.GetNodeID(),arr.GetMaxmumInstanID());
}   

void InstanceID::InsertKeys(const epaxos_client::OperationKVArray & arrvalues) {
    arrvalues.GetArrKeys(keys_);
}

bool InstanceID::NewerAndUpdate(const InstanceID&a){
    if(a.state_ < state_ ){
        return false;
    }
    if(a.seqid_ < seqid_) {
        return false;
    }
    assert(a.deps_.size()== deps_.size());
    seqid_ =  a.seqid_ ;
    state_ = a.state_;
    std::copy(a.deps_.begin(),a.deps_.end(),deps_.begin());
    return true;
}

std::string InstanceID::GetDetailInfo()const{
    std::strstream st;
    st << "[SeqID: " << seqid_ << "] ";
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


InstanceID* InstanceArrIds::GetMaxmumInstanID()const{
    std::map<uint64_t,InstanceID*>::const_reverse_iterator riter = arryins_.rbegin();

    if(riter == arryins_.rend()){
        return nullptr;
    }
    return riter->second;
}

std::string InstanceArrIds::GetArrayDetail()const{
    std::strstream st;
    st << "[Node: " << nodeID_ <<"] ";
    st << "[CurMaxIns: " << curMaxInsId_ << "] "<< std::endl;
    std::for_each(arryins_.begin(),arryins_.end(),[&](const std::pair <uint64_t,InstanceID *>  & value){
        st << "      " << "[InsID: "<<value.first <<"] : " << value.second->GetDetailInfo() << std::endl;
    });
    return st.str();
}

/**
 * @brief 获取最新的instance的信息对seq进行递增
 * 
 * @param nodesize 
 * @return InstanceID* 
 */
InstanceID* InstanceArrIds::GenNewInstanceID(uint32_t nodesize){
    //lock
    InstanceID *last = GetMaxmumInstanID();
    assert(last!=nullptr);
    curMaxInsId_++;//并发问题


    //获取当前seq的最大值

    //获取上一个ins的seq和insid
    InstanceID *ins = new InstanceID(curMaxInsId_,nodeID_,nodesize,last->GetSeqId()+1);
    assert(ins!=nullptr);
    //获取上一个ins的seq
    
    arryins_[curMaxInsId_]=ins;
    //unlock
    return ins;
}

ResCode InstanceArrIds::InserNewIns(const InstanceID & ins){
    auto iter = arryins_.find(ins.GetInstId());
    if(iter == arryins_.end()){
        //直接拷贝
        arryins_[ins.GetInstId()]= new InstanceID(ins);
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
    //不能是自己节点提出的值
    assert(!localArray_->IsLoacalIns(inswap));

    int indexNode = inswap.GetNodeID();
    //找到对应的队列
    assert(indexNode < inslist_.size() );
    return  inslist_.at(indexNode).InserNewIns(inswap);
}

/**
 * @brief value进行存储，key保存在ins中
 * 
 * @param keys 
 * @param values 
 * @return InstanceID* 
 */
InstanceIDSwap InstanceNode::GenLocalNewInstanceId(const epaxos_client::OperationKVArray & arrvalues){
    InstanceID*  p_ins = localArray_->GenNewInstanceID(nodeSize_);
    //写入db key values

    //更新deps
    std::for_each(inslist_.begin(),inslist_.end(),[&](const InstanceArrIds&ins){
        if(ins.IsLocalArr()){
            return;
        }
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
    std::strstream st;
    st<< "DebugPrintInstanceNode getdetail: " <<std::endl;
    std::for_each(inslist_.begin(),inslist_.end(),[&st](const InstanceArrIds &a ){
        st << a.GetArrayDetail() <<std::endl;
    });
    return st.str();
}

};