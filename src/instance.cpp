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

IfChange Instance::RefreshDeps(NodeID nid, InsID t){
    return  deps_.UpdateOne(nid,t);
}

IfChange Instance::Update(NodeID nid,const Instance & a){
    IfChange res;
    
    res.Refresh(seqid_.Update(a.seqid_));

    res.Refresh(deps_.UpdateArray(a.deps_));
    return res;
}

bool Instance::operator ==(const Instance &a )const{
    bool res = seqid_ == a.seqid_;
    if(!res){ return res;}
    res = state_ == a.state_;
    if(!res){ return res;}
    res= deps_ == a.deps_;
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
    ResCode res(0);
    if (!(curMaxInsId_ < ins.GetInsID())) {
        curMaxInsId_.DoUpdate(ins.GetInsID());
        res.SetExist();
    }
    auto iter = clins_.find(ins.GetInsID());
    if(iter != clins_.end()){
        iter->second->Update(ins.GetNodeID(),*ins.GetInsPtr());
    }else {
        //这里插入一个值
        clins_[ins.GetInsID()] = new Instance(*ins.GetInsPtr());
    }
    return  res;
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

//双向更新
IfChange InstanceCollector::RefreshLocalIns(const InstanceSwap & st){
    //更新本地的deps

    InsID  t= st.GetInsPtr()->GetDepsID(id_);

    //这个值就是用来更新本地的，如果本地更新失败，就更新进来的值
    IfChange res = curMaxInsId_.DoUpdate(t);
    
    return res;
}

IfChange InstanceCollector::MutualRefreshSwap(InstanceSwap & st){
    IfChange res = RefreshLocalIns(st);
    st.UpdateDepsIns(id_,curMaxInsId_);

    std::cout<< "InstanceCollector: " << id_.Value64()<< " maxins:" << curMaxInsId_.Value64()<< " SWAP:" << st.GetDetailInfo()<<std::endl;
    return res;
}

std::string InstanceCollector::GetState()const{
    std::stringstream st;
    st << "[Node: " << id_.Value64() << " CurMaxIns: " << curMaxInsId_.Value64() << "] ";
 
    return st.str();
}

InstanceSwap InstanceCollector::GetTragetIns(const InstanceSwap & inswap) const{
    std::map<InsID,Instance*>::const_iterator iter =  clins_.find(inswap.GetInsID());
    if(iter == clins_.end()){
        assert(false);
        return InstanceSwap();
    } 
    return InstanceSwap(id_,iter->first,iter->second);
}

std::string InstanceCollector::GetLocalInsIds()const{
    std::stringstream st;
    st <<"[Node: "<< id_.Value64() << " all inst CurMaxIns: "<<curMaxInsId_.Value64() << ":"<< std::endl;
    for(auto iter=clins_.begin();iter!=clins_.end();iter++){
        st <<"  insid:"<<iter->first.Value64() << " " << iter->second->DebugInfo()<<std::endl;
    }
    st<<"END]"<<std::endl;
    return st.str();
}

std::string InstanceCollector::Include(const InstanceCollector &mt)const{
    std::stringstream rest;
    std::map<InsID,Instance*>::const_iterator iter = clins_.begin();
    for (;iter != clins_.end();iter++){
       std::map<InsID,Instance*>::const_iterator ist = mt.clins_.find(iter->first);
       if(ist != mt.clins_.end()){
            if (!(*ist->second == *iter->second)) {
                rest<<"[not equal collectorID: "<< id_.Value64()<< " insID:" << iter->first.Value64()<<"]";
            }
       }
    }
    return rest.str();
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

    //如果ins已经存在，就直接更新seq和本地的ins
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


/**
 * @brief 如果不存在就插入，如果存在就更新本地，如果本地超前，就更新对方
 * 
 * @param inswap 
 * @return ResCode 
 */
ResCode InstanceNode::PreAccept(InstanceSwap & inswap){
    epaxos::ResCode res;

    //如果是本地的
    if(inswap.GetNodeID() == localNodeId_) {
        return ReFreshLocal(inswap);
    }
    //更新max ins
    assert(inswap.GetNodeID().Value64() < inslist_.size());

    //先插入，写入本地事件ins
    res = inslist_.at(inswap.GetNodeID().Value64()).InsertAndUpdate(inswap);

    //dep也需要更新到所有inslis节点上
    std::for_each(inslist_.begin(),inslist_.end(),[&](InstanceCollector & ins){
        if(inswap.GetNodeID() == ins.GetNodeID()){
            return;
        }
        res.Refresh(ins.MutualRefreshSwap(inswap));
    });

    if(res.IsChange()){
         //落盘 todo
    }

    inswap.SetToNode(localNodeId_);

    //如果本地存在，就更新最大seq，不需要得到最新的
    
    //seq 先去更新本地，如果进入的没有发生变化，我再去更新你的
    if(res.IsExist()){
        std::cout<<"PreAccept" << localNodeId_.Value64() << " exist"<<std::endl;
        seq_.Update(inswap.GetInsPtr()->GetSeqId());
    } else {

        std::cout<<"PreAccept " << localNodeId_.Value64() << " not exist"<<std::endl;
        seq_.MutualUpdate(inswap.GetMutalInsPtr()->GetSeqIdReference());
    }
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


std::string InstanceNode::DebugPrintInstanceNode()const{
    std::stringstream st;
    st<< "[CurNode: " << localNodeId_.Value64() << " MaxSeq: " << seq_.Value64()<<"] ";
    std::for_each(inslist_.begin(),inslist_.end(),[&st](const InstanceCollector &a ){
        st << a.GetState() <<" ";
    });
    return st.str();
}

std::string InstanceNode::GetAllCollectorInsInfo()const {
    std::stringstream st;
    st<< "[CurNode: " << localNodeId_.Value64() << " MaxSeq: " << seq_.Value64()<< std::endl;;
    std::for_each(inslist_.begin(),inslist_.end(),[&](const InstanceCollector & a){
        st <<"  "<< a.GetLocalInsIds();
    });
    st << " ]"<<std::endl;
    return  st.str();
}

ResCode InstanceNode::Include(const InstanceNode &nd) const {
    assert(inslist_.size()==nd.inslist_.size());
    std::stringstream st;
    size_t n = inslist_.size();
    for(size_t i=0;i<n;i++){
        std::string tmp = inslist_[i].Include(nd.inslist_[i]);
        if(tmp.size() >0 ){
            st << "[CurNode:"<< localNodeId_.Value64() << " compareid:"<<nd.localNodeId_.Value64() << "][subnode:" << inslist_[i].GetNodeID().Value64() << "]"<<tmp <<"]" << std::endl;
        }
    }
    return ResCode(st.str());
}

epaxos::InstanceSwap InstanceNode::GetTargetIns(const InstanceSwap & inswap)const {
    return  inslist_[inswap.GetNodeID().Value64()].GetTragetIns(inswap);
}

};