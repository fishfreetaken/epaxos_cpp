

#ifndef __EPAXOS_INSTANCE__
#define __EPAXOS_INSTANCE__

#include "../remote/operationKV.h"
#include "../include/comminclude.h"
#include "status.h"
#include "../include/nodecfg.h"
#include "rescode.h"
#include "cluster.h"
#include "../include/arraymanage.h"

namespace epaxos {

class MutexSeqID : public IndexItem{
public:
    MutexSeqID(IndexItem a):IndexItem(a){}
    MutexSeqID(){}
    IfChange Update(const MutexSeqID &t ){
        //lock 
        return DoUpdate(t);
        //unlock
    }
private:
};

//只能单调递增
class InsID : public IndexItem {
public:
    InsID(uint64_t a):IndexItem(a){}
    InsID():IndexItem(){}
};

class DepsIDs : public ArrayManage<InsID>{
public:
    DepsIDs(NodeSize s):ArrayManage<InsID>(s){}
};

/**
 * @brief 通过insid进行索引
 * 
 */
class Instance {
public:
    Instance(const DepsIDs& s, MutexSeqID seqid , const epaxos_client::OperationKVArray & arrvalues);
    Instance(const Instance & a);
    ~Instance(){
        //std::cout<<"descrpter "<< seqid_.Value64() << std::endl;
    }

    /**
     * @brief 更新本提议的seq值，同时插入并更新当前deps
     * 
     * @param ins 
     */
    IfChange Update(NodeID nid,const Instance &a);

    void RefreshSeqID(MutexSeqID s);
    void RefreshDeps(NodeID nid, InsID t);
    /**
     * @brief Get the Detail Info object 打印自身详情
     * 
     * @return std::string 
     */
    std::string DebugInfo()const;

    InsID GetDepsID(NodeID &id)const{ return deps_.GetPosValue(id);}

    MutexSeqID GetSeqId()const{return seqid_;}

private:
    bool IsBehind(const Instance &a) const; //这个值是否落后
private:
    MutexSeqID seqid_;                                  //全局唯一
    WorkStatus state_;                                  //状态
    std::vector<std::string> arrkeys_;                  //根据key+instID索引请求的value
    DepsIDs deps_;                                      //依赖 insid
};

class IdentifyIns {
public: 
    IdentifyIns(NodeID nodeid,  InsID insid ):fromNode_(nodeid),insId_(insid){}
    NodeID GetNodeID() const{ return fromNode_;}
    InsID GetInsID() const { return insId_;}

protected:
    std::string GetIdentifyInfo(){
        std::stringstream st;
        st<<"[NodeID:"<<fromNode_.Value64()<<"]" <<" [InsId:" << insId_.Value64()<<"]";
        return st.str();
    }
protected:
    NodeID fromNode_;
    InsID insId_;
    IfChange ch_;
};


class InstanceSwap : public IdentifyIns{
public:
    InstanceSwap(NodeID nodeid,  InsID insid ,const Instance *a):IdentifyIns(nodeid,insid){
        ins_ = std::make_shared<Instance> (*a);
    }

    InstanceSwap(const InstanceSwap &a ):IdentifyIns(a){
        ins_ = a.ins_;
    }
    InstanceSwap():IdentifyIns(0,0){}
    //InstanceSwap(NodeID nodeid,  Instance *a, const epaxos_client::OperationKVArray * pkv):IdentifyIns(nodeid,InsID(0)),ins_(a){}

    const Instance *GetInsPtr()const{return ins_.get();}

    std::string GetDetailInfo(){
         std::stringstream st;
         st << GetIdentifyInfo() << ins_->DebugInfo();
         return st.str();
    }

    std::shared_ptr<Instance> GetMutalInsPtr(){return ins_;}

    InstanceSwap New() {
        InstanceSwap t(*this);
        t.ins_ = std::make_shared<Instance>(*(this->ins_.get()));
        return t;
    }
private:
    std::shared_ptr<Instance> ins_;
};


class InstanceCollector {
public:
    InstanceCollector(NodeID i):id_(i.Value64()){}

    /**
     * @brief 插入新的ins 带有一定的淘汰策略，必须先落盘
     * 
     * @param ins 
     * @return ResCode 
     */
    ResCode InsertAndUpdate(const InstanceSwap & ins);

    /**
     * @brief 从本地更新外来的值
     * 
     * @param st 
     * @return IfChange 
     */
    MutexSeqID RefreshFromLastIns(InstanceSwap & st)const;

    InstanceSwap GenOne(const DepsIDs& s, MutexSeqID seqid , const epaxos_client::OperationKVArray & arrvalues);

    NodeID GetNodeID()const {return id_;}

    IfChange RefreshLocalIns(const InstanceSwap & st);

    InstanceCollector  operator = (int t){return std::move(InstanceCollector(NodeID(t))); }

    InsID GetMaxInsId()const {return curMaxInsId_;}

    std::string GetState()const;
private:
    const Instance* GetLastOne() const;

private:
    InsID curMaxInsId_;  //原子自增
    NodeID id_;

    //lru cache
    std::map<InsID,Instance*> clins_; //有序队列，快速索引查找
};

class InstanceNode {
public:
    InstanceNode(NodeID t,NodeSize s); //本地的node

    //生成inst事件并保存本地
    InstanceSwap GenNewInstance(const epaxos_client::OperationKVArray & arrvalues);

    /**
     * @brief 从其他node同步写入进来，如果不存在就插入，如果存在就更新本地，同时也要对这个值进行更新
     * 只更新，不前进
     * 
     * @return ResCode 
     */
    ResCode MutualManageIns(InstanceSwap & inswap);

    std::string DebugPrintInstanceNode();

    ResCode ReFreshLocal(const InstanceSwap &inswap);
private:
    IfChange MaxmumSeqID(MutexSeqID t){ return seq_.Update(t);}

    NodeSize GetArrSize()const{ return NodeSize(inslist_.size());}  

    DepsIDs GetArrDeps()const;

    ResCode ReFreshRemote(InstanceSwap & inswap) const;

    /**
     * @brief 插入或者更新本地，更新ins，更新seq
     * 
     * @param inswap 
     * @return ResCode 
     */
    //ResCode ReFreshLocal(const InstanceSwap & inswap);  

private:
    InstanceCollector *localArray_;
    std::vector<InstanceCollector > inslist_;

    NodeID localNodeId_;

    MutexSeqID seq_; //不落盘
};

}
#endif