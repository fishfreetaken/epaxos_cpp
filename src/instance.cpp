#include "instance.h"

#include <numeric>
namespace  epaxos {

Instance::Instance(const DepsIDs& s,MutexSeqID seqid ,const epaxos_client::OperationKVArray & arrvalues):seqid_(seqid),deps_(s){
    arrvalues.GetArrKeys(arrkeys_);
}

Instance::Instance(const Instance &a):seqid_(a.seqid_),state_(a.state_),deps_(a.deps_){}

bool Instance::IsBehind(const Instance &a) const{
    if (state_ < a.state_ ) {
        return false;
    }
    if (a.seqid_ < seqid_){
        return true;
    }
    return false;
}

void Instance::RefreshSeqID(MutexSeqID s){
    seqid_.RefeshLocToAhead(s);
}

void Instance::RefreshDeps(NodeID nid, InsID t){
    deps_.UpdateOne(nid,t);
}

IfChange Instance::Update(NodeID nid,const Instance & a){
    IfChange res;
    
    res.Refresh(seqid_.RefeshLocToAhead(a.seqid_));

    res.Refresh(deps_.UpdateArray(a.deps_));
    return res;
}

std::string Instance::DebugInfo()const{
    std::stringstream st;
    
    st << " [SeqID: " << seqid_.Value64() << "] ";
    st << " [Deps: "<< deps_.DebugInfo() << "]";
    st << " [KEYS: ";

    std::for_each(arrkeys_.begin(),arrkeys_.end(),[&](const std::string &a ){
        st << a;
    });
    st << "]";
    return st.str();
}


ResCode InstanceCollector::InsertAndUpdate(const InstanceSwap &ins){
    //插入不能时本地的node
    auto iter = clins_.find(ins.GetInsID());
    if(iter != clins_.end()){
        iter->second->Update(ins.GetNodeID(),*ins.GetInsPtr());
    }else {
        //这里插入一个值
        clins_[ins.GetInsID()] = new Instance(*ins.GetInsPtr());
    }

    curMaxInsId_.DoUpdate(ins.GetInsPtr()->GetSeqId());
    return  ResCode(0);
}


const Instance* InstanceCollector::GetLastOne()const{
    InsID tmpMaxInsId = GetMaxInsId();
    std::map<InsID,Instance*>::const_iterator iter = clins_.find(tmpMaxInsId);
    if (iter == clins_.end()) {
        //todo 在磁盘文件中寻找 ,后边就交给leveldb
        return nullptr;
    }
    return iter->second;
}

MutexSeqID InstanceCollector::RefreshFromLastIns(InstanceSwap & st)const {
    MutexSeqID res;
    /*
    const Instance *tp = GetLastOne();
    if(tp == nullptr){
        return res;
    }
    */
    //获取最后一个insid
    //st.GetChangeInsPtr()->Update(id_,*tp);
    st.GetMutalInsPtr()->RefreshDeps(id_,curMaxInsId_);
    return res;
}

InstanceSwap InstanceCollector::GenOne(const DepsIDs& s, MutexSeqID seqid , const epaxos_client::OperationKVArray & arrvalues){
    //原子递增
    InsID mt(curMaxInsId_.Inc().Value64());
    Instance* p_ins = new Instance(s,seqid,arrvalues);

    //todo： 写入db

    //更新本地
    clins_[mt] = p_ins;

    return InstanceSwap(id_,mt,p_ins);
}

IfChange InstanceCollector::RefreshLocalIns(const InstanceSwap & st){
    //更新本地的deps

    InsID  t= st.GetInsPtr()->GetDepsID(id_);

    curMaxInsId_.DoUpdate(t);

    return false;
}

std::string InstanceCollector::GetState()const{
    std::stringstream st;
    st << "[Node: " << id_.Value64() << " CurMaxIns: " << curMaxInsId_.Value64() << "] ";
 
    return st.str();
}

InstanceNode::InstanceNode(NodeID t,NodeSize s):localNodeId_(t),seq_(0){

    assert(s.Value64()>0);
    assert(t.Value64() < s.Value64());

    for(size_t i =0;i<s.Value64();i++){
        inslist_.push_back(InstanceCollector(i));
    }

    assert(inslist_.size() ==  s.Value64());

    localArray_ = &inslist_[t.Value64()];
}


ResCode InstanceNode::ReFreshLocal(const InstanceSwap & inswap){
    ResCode res(0);

    //更新max ins
    assert(inswap.GetNodeID().Value64() < inslist_.size());

    //先落盘
    res = inslist_.at(inswap.GetNodeID().Value64()).InsertAndUpdate(inswap);

    //dep也需要更新到所有inslis节点上
    std::for_each(inslist_.begin(),inslist_.end(),[&](InstanceCollector & ins){
        res.Refresh(ins.RefreshLocalIns(inswap));
    });

    //更新maxseq
    res.Refresh(seq_.Update(inswap.GetInsPtr()->GetSeqId()));
    return res;
}

ResCode InstanceNode::ReFreshRemote(InstanceSwap & inswap)const{
    ResCode res;
    inswap.GetMutalInsPtr()->RefreshSeqID(seq_);
    std::for_each(inslist_.begin(),inslist_.end(),[&](const InstanceCollector &  ins){
        ins.RefreshFromLastIns(inswap);
    });
    return res;
}

/**
 * @brief 如果不存在就插入，如果存在就更新本地，如果本地超前，就更新对方
 * 
 * @param inswap 
 * @return ResCode 
 */
ResCode InstanceNode::MutualManageIns(InstanceSwap & inswap){

    ResCode res = ReFreshRemote(inswap);

    res.Refresh(ReFreshLocal(inswap));
    return res;
}

/**
 * @brief value进行存储，key保存在ins中
 * 
 * @param keys 
 * @param values 
 * @return Instance* 
 */
InstanceSwap InstanceNode::GenNewInstance(const epaxos_client::OperationKVArray & arrvalues){

    //自增insid并将所有kv事件写入硬盘
    
    //获取seq和ins
    MutexSeqID tmpMaxSeq (seq_.Inc()); //and snapshot
    DepsIDs tmpDep = GetArrDeps();

    //todo 写入db key values, 自增之后写入
    InstanceSwap sp = localArray_->GenOne(tmpDep,tmpMaxSeq,arrvalues);

    return InstanceSwap(sp);
}


DepsIDs InstanceNode::GetArrDeps()const {
    DepsIDs res(GetArrSize());
    std::for_each(inslist_.begin(),inslist_.end(),[&](const InstanceCollector & ins){
        InsID id = ins.GetMaxInsId();
        res.UpdateOne(ins.GetNodeID(),id);
    });
    return res;
}


std::string InstanceNode::DebugPrintInstanceNode(){
    std::stringstream st;
    st<< "[CurNode: " << localNodeId_.Value64() << " MaxSeq: " << seq_.Value64()<<"] ";
    std::for_each(inslist_.begin(),inslist_.end(),[&st](const InstanceCollector &a ){
        st << a.GetState() <<" ";
    });
    return st.str();
}

};