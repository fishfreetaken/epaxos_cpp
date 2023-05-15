

#ifndef __EPAXOS_INSTANCE__
#define __EPAXOS_INSTANCE__

#include "../remote/operationKV.h"
#include "../include/comminclude.h"
#include "status.h"
#include "rescode.h"
namespace epaxos {

class InstanceArrIds;
class InstanceID {
public:
    InstanceID(uint64_t insId,uint32_t nodeid,uint32_t insNodeNum,uint64_t seq);

    uint64_t GetInstId() const {return insid_;}
    uint64_t GetSeqId() const {return seqid_;}
    /**
     * @brief 更新本提议的seq值，同时插入并更新当前deps
     * 
     * @param ins 
     */
    void UpdateSeqAndInsertDep(uint32_t nodeid, const InstanceID * ins);
    void UpdateSeqAndInsertDep(const InstanceArrIds &arr);

    void InsertKeys(const epaxos_client::OperationKVArray & arrvalues);

    /**
     * @brief Get the Detail Info object 打印自身详情
     * 
     * @return std::string 
     */
    std::string GetDetailInfo()const;

    bool NewerAndUpdate(const InstanceID&a);

private:
    bool IsBehind(const InstanceID &a)const{return a.state_ < state_;  } //这个值是否落后

private:
    uint64_t insid_;                        //事件id
    uint64_t seqid_;                        //全局唯一
    WorkStatus state_;                      //状态
    std::vector<std::string> keys_;         //根据key+instID索引请求的value
    std::vector<uint64_t> deps_;            //依赖 insid
};

class InstanceIDSwap : public InstanceID{
public:
    InstanceIDSwap(uint32_t nodeid, InstanceID &a):InstanceID(a),nodeid_(nodeid){
    }
    //InstanceIDSwap(uint32_t nodeid,uint32_t insid,uint32_t  insNodeNum):InstanceID(insid,insNodeNum),nodeid_(nodeid){}
    uint32_t GetNodeID()const{return nodeid_;}
private:
    uint32_t nodeid_;
};

class InstanceArrIds {
public:
    InstanceArrIds(uint32_t nodeId):nodeID_(nodeId),curMaxInsId_(0),isLocalArray_(false){
        Init();
    }
    void Init(){
        arryins_[0]= new InstanceID(0,nodeID_,nodeID_+1,0);
    }


    void SetLocalArr(bool islocal){isLocalArray_=islocal ; }

    //从存储中加载出对应的配置

    InstanceID* GetMaxmumInstanID()const;

    InstanceID* GenNewInstanceID(uint32_t nodesize);
    /**
     * @brief 插入新的ins 带有一定的淘汰策略，必须先落盘
     * 
     * @param ins 
     * @return ResCode 
     */
    ResCode InserNewIns(const InstanceID & ins);

    uint32_t GetNodeID()const{return nodeID_;}


    bool IsLoacalIns(const InstanceIDSwap& ins)const { return ins.GetNodeID() == nodeID_;}

    bool IsLocalArr()const {return isLocalArray_;}

    std::string GetArrayDetail() const;

private:
    uint32_t nodeID_;
    uint64_t curMaxInsId_;  //原子自增
    bool isLocalArray_;
    std::map<uint64_t,InstanceID*> arryins_; //有序队列，快速索引查找
};

class InstanceNode {
public:
    InstanceNode(uint32_t locNodeID, uint32_t nodeNum);

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
    std::vector<InstanceArrIds> inslist_;  //实例列表
    InstanceArrIds *localArray_;
    uint32_t nodeSize_;
};

}

#endif