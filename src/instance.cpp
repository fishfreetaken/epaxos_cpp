#include "instance.h"

#include <numeric>
namespace  epaxos {

Instance::Instance(NodeSize s,MutexSeqID seqid ,const epaxos_client::OperationKVArray & arrvalues):seqid_(seqid),deps_(s){
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

IfChange Instance::Update(NodeID nid,const Instance & a){
    IfChange res;
    if(IsBehind(a)){
        return res;
    }
    
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
    return  ResCode(0);
}


const Instance* InstanceCollector::GetLastOne()const{
    InsID tmpMaxInsId = curMaxInsId_;
    std::map<InsID,Instance*>::const_iterator iter = clins_.find(tmpMaxInsId);
    if (iter == clins_.end()) {
        //todo 在磁盘文件中寻找 ,后边就交给leveldb
        return nullptr;
    }
    return iter->second;
}

IfChange InstanceCollector::RefreshFromLastIns(InstanceSwap & st)const {
    IfChange res;
    const Instance *tp = GetLastOne();
    if(tp == nullptr){
        return res;
    }
    //获取最后一个insid 
    res = st.GetChangeInsPtr()->Update(id_,*tp);
    return res;

}

InstanceSwap InstanceCollector::GenOne(NodeSize s, MutexSeqID seqid , const epaxos_client::OperationKVArray & arrvalues){
    //原子递增
    InsID mt(curMaxInsId_.Inc());
    Instance* p_ins = new Instance(s,seqid,arrvalues);

    //todo 写入db

    auto iter = clins_.find(mt);
    assert(iter == clins_.end());

    InstanceSwap swap = InstanceSwap(id_,mt,p_ins);

    RefreshFromLastIns(swap);

    //更新本地
    clins_[mt] = p_ins;

    return swap;
}

std::string InstanceCollector::GetArrayDetail()const{
    std::stringstream st;
    st << "[Node: " << 1 <<"] ";
    st << "[CurMaxIns: " << 1 << "] "<< std::endl;
 
    return st.str();
}

InstanceNode::InstanceNode(NodeID t,NodeSize s):localNodeId_(t),seq_(){

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

    //更新maxseq
    res.Refresh(seq_.Update(inswap.GetInsPtr()->GetSeqId()));
    return res;
}

ResCode InstanceNode::ReFreshRemote(InstanceSwap & inswap)const{
    ResCode res; 
    InstanceSwap swp(inswap);
    std::for_each(inslist_.begin(),inslist_.end(),[&](const InstanceCollector &  ins){
        res.Refresh( ins.RefreshFromLastIns(swp));
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

    ResCode res = ReFreshLocal(inswap);

    if (res.IsChange()){
        //本地的值已经发生了变化
        return res;
    }

    return ReFreshRemote(inswap);
}

/**
 * @brief value进行存储，key保存在ins中
 * 
 * @param keys 
 * @param values 
 * @return Instance* 
 */
InstanceSwap InstanceNode::GenNewInstance(const epaxos_client::OperationKVArray & arrvalues){

    seq_.Inc(); //and snapshot
   
    //自增insid并将所有kv事件写入硬盘
    
    //todo 写入db key values, 自增之后写入
    InstanceSwap sp = localArray_->GenOne(GetArrSize(),seq_,arrvalues);
    //std::cout <<"before update:" << sp.GetChangeInsPtr()->DebugInfo() <<std::endl;
    //更新deps
    std::for_each(inslist_.begin(),inslist_.end(),[&](const InstanceCollector &  ins){
        if( ins.IsLocalCollector(localNodeId_)){
            //这里逻辑已经过滤了本身的nodeid
            //std::cout << "local collect: " << localNodeId_.Value64() << std::endl;
            return;
        }
        IfChange res =  ins.RefreshFromLastIns(sp);
        if (res.IsChange()){
            //std::cout<<"change id:"<< ins.GetNodeID().Value64() <<std::endl;
        }
        //std::cout <<"after update:"<< ins.GetNodeID().Value64() << sp.GetChangeInsPtr()->DebugInfo() <<std::endl;
    });
    //TODO 固化写入文件

    return InstanceSwap(sp);
}

std::string InstanceNode::DebugPrintInstanceNode(){
    std::stringstream st;
    st<< "DebugPrintInstanceNode getdetail: " <<std::endl;
    std::for_each(inslist_.begin(),inslist_.end(),[&st](const InstanceCollector &a ){
        st << a.GetArrayDetail() <<std::endl;
    });
    return st.str();
}

};