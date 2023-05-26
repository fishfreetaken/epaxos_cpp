

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
    InsID(IndexItem a):IndexItem(a){}
    InsID():IndexItem(){}

private:
};

class DepsIDs : public ArrayManage{
public:
    DepsIDs(NodeSize s):ArrayManage(s){}
};

/**
 * @brief 通过insid进行索引
 * 
 */
class Instance {
public:
    Instance(NodeSize s, MutexSeqID seqid , const epaxos_client::OperationKVArray & arrvalues);
    Instance(const Instance & a);

    /**
     * @brief 更新本提议的seq值，同时插入并更新当前deps
     * 
     * @param ins 
     */
    IfChange Update(NodeID nid,const Instance &a);
    /**
     * @brief Get the Detail Info object 打印自身详情
     * 
     * @return std::string 
     */
    std::string DebugInfo()const;

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
private:
    NodeID fromNode_;
    InsID insId_;
    IfChange ch_;
};


class InstanceSwap : public IdentifyIns{
public:
    InstanceSwap(NodeID nodeid,  InsID insid ,Instance *a):IdentifyIns(nodeid,insid),ins_(a){}
    //InstanceSwap(NodeID nodeid,  Instance *a, const epaxos_client::OperationKVArray * pkv):IdentifyIns(nodeid,InsID(0)),ins_(a){}

    const Instance *GetInsPtr()const{return ins_.get();}

    std::shared_ptr<Instance> GetChangeInsPtr(){return ins_;}

 
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

    std::string GetArrayDetail() const;

    /**
     * @brief 从本地更新外来的值
     * 
     * @param st 
     * @return IfChange 
     */
    IfChange RefreshFromLastIns(InstanceSwap & st)const ;

    InstanceSwap GenOne(NodeSize s, MutexSeqID seqid , const epaxos_client::OperationKVArray & arrvalues);

    NodeID GetNodeID()const {return id_;}

    InstanceCollector  operator = (int t){return std::move(InstanceCollector(NodeID(t))); }

    bool IsLocalCollector(const NodeID &id) const{ return id == id_;}
private:
    const Instance* GetLastOne() const;

    Instance * Find(InsID id);
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
     * 
     * @return ResCode 
     */
    ResCode MutualManageIns(InstanceSwap & inswap);

    ResCode ReFreshRemote(InstanceSwap & inswap) const;

    /**
     * @brief 插入或者更新本地，更新ins，更新seq
     * 
     * @param inswap 
     * @return ResCode 
     */
    ResCode ReFreshLocal(const InstanceSwap & inswap) ;

    /**
     * @brief TODO 将当前状态落盘
     * 
     * @return ResCode 
     */
    ResCode SnapShot();

    std::string DebugPrintInstanceNode();

private:
    IfChange MaxmumSeqID(MutexSeqID t){ return seq_.Update(t);}

    NodeSize GetArrSize()const{ return NodeSize(inslist_.size());}

private:
    InstanceCollector *localArray_;
    std::vector<InstanceCollector > inslist_;

    NodeID localNodeId_;

    MutexSeqID seq_; //不落盘
};

}
#endif