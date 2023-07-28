
#ifndef ___EPAXOS_GLOBAL_NODE_CONFIG__ 

#define ___EPAXOS_GLOBAL_NODE_CONFIG__
#include "rescode.h"
#include <set>
namespace epaxos {

/**
 * @brief 初始化更新之后就不能发生改变
 * 
 */
template<typename T>
class ConstIndexValue {
public:
    ConstIndexValue() = delete;
    ConstIndexValue(T t):val_(t){}

    T Value()const{return val_;}

    bool operator ==(const ConstIndexValue &a)const { return a.val_ == val_;}
private:
    T val_;
};

class NodeSize : public ConstIndexValue<uint32_t>{
public:
    NodeSize()=delete;
    NodeSize(size_t t):ConstIndexValue<uint32_t>(t){};
private :
};

//有的应该也可以用一个集群来进行管理
class NodeID : public ConstIndexValue<int>{
public:
    NodeID()=delete;
    NodeID(int t):ConstIndexValue<int>(t){};
private:
};

class NodeCfg : public NodeID{

private:
    uint32_t ip_;
    uint32_t port_;
};

class CConf{
public:
    CConf(size_t s, NodeID n):css_(s),localNodeID_(n){}
    size_t GetSize()const {return css_; }
    NodeID GetLocalNode(){return localNodeID_;}
private:
    std::map<NodeID,NodeCfg> allNodeid_;
    size_t css_; //cluster size

    std::set<NodeID> fastCluster;
    NodeID localNodeID_;
    
};

};
#endif
