
#ifndef ___EPAXOS_GLOBAL_NODE_CONFIG__ 

#define ___EPAXOS_GLOBAL_NODE_CONFIG__
#include "arraymanage.h"
#include "rescode.h"
namespace epaxos {

class NodeSize : public IndexValue{
public:
    NodeSize()=delete;
    NodeSize(size_t t):IndexValue(t){};
private :
};

//有的应该也可以用一个集群来进行管理
class NodeID : public IndexValue{
public:
    NodeID()=delete;
    NodeID(size_t t):IndexValue(t){};
private:
};

class NodeCfg : public NodeID{

private:
    uint32_t ip_;
    uint32_t port_;
};

class ClusterManage{

public:
    

};

};
#endif
