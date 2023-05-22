

#ifndef __EPAXOS_INSTANCE__
#define __EPAXOS_INSTANCE__

#include "../remote/operationKV.h"
#include "../include/comminclude.h"
#include "status.h"
#include "../include/nodecfg.h"
#include "rescode.h"
#include "cluster.h"

namespace epaxos {

class MutexSeqID : public Index{
public:
private:
};

//只能单调递增
class InsID : public Index{
public:
private:
};

class DepsIDs : private ArrayManage<InsID>{
public:
    DepsIDs(NodeSize &s):ArrayManage<InsID>(s.Value()){}
    /**
     * @brief 由deps来更新deps
     * 
     * @param a 
     */
    void Update(NodeID idx,const InsID &a);
};

class Instance {
public:
    Instance(NodeSize s):deps_(s){}
    Instance(NodeSize s, const epaxos_client::OperationKVArray & arrvalues);

    /**
     * @brief 更新本提议的seq值，同时插入并更新当前deps
     * 
     * @param ins 
     */
    void Update(const NodeID nid, const MutexSeqID &seq,const InsID &ins);

    bool Update(const Instance &a);
    /**
     * @brief Get the Detail Info object 打印自身详情
     * 
     * @return std::string 
     */
    std::string GetDetailInfo()const;


private:
    bool IsBehind(const Instance &a)const{return a.state_ < state_;} //这个值是否落后

private:
    MutexSeqID seqid_;                                  //全局唯一
    WorkStatus state_;                                  //状态
    std::vector<std::string> arrkeys_;                  //根据key+instID索引请求的value
    DepsIDs deps_;                                      //依赖 insid
};

class InstanceIDSwap : public Instance{
public:
    InstanceIDSwap(const NodeID & nodeid, const InsID& ins ,Instance &a):Instance(a),fromNode_(nodeid),insId_(ins){}

    std::string DetailInfoAddNode(){ return "[Node:"+std::to_string(fromNode_)+"]" + GetDetailInfo(); }
private:
    NodeID fromNode_;
    InsID insId_;
};

class InstanceArrIds: public ClusterMember {
public:
    InstanceArrIds();

    //从存储中加载出对应的配置

    Instance* GetMaxmumInstanID()const;

    Instance* GenNewInstanceID(uint32_t nodesize);

    /**
     * @brief 插入新的ins 带有一定的淘汰策略，必须先落盘
     * 
     * @param ins 
     * @return ResCode 
     */
    ResCode InserNewIns(const Instance & ins);

    std::string GetArrayDetail() const;

private:    
    uint64_t IncLocalMaxInsID();

private:
    InsID curMaxInsId_;  //原子自增

    //lru cache
    std::map<NodeID,Instance*> arryins_; //有序队列，快速索引查找
};

class InstanceNode : private ArrayManage<InstanceArrIds> {
public:
    InstanceNode(NodeID t); //本地的node

    //生成inst事件并保存本地
    InstanceIDSwap GenLocalNewInstanceId(const epaxos_client::OperationKVArray & arrvalues); 

    //生成一条空的用来进行占位
    InstanceIDSwap GenLocalNewInstanceId();

    /**
     * @brief 告诉我是重复的值还是说一个新值增加成功
     * 
     * @return ResCode 
     */
    ResCode InsertNewInst(const InstanceIDSwap & inswap);

    std::string DebugPrintInstanceNode();

private:
    InstanceArrIds *localArray_;
    NodeID localNodeID_;
};

}

#endif