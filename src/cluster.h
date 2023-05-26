#ifndef __EPAXOS_CLUSTER_MANAGE__
#define __EPAXOS_CLUSTER_MANAGE__
#include "nodecfg.h"
namespace epaxos{

//管理整个集群的
class ClusterMember{
public: 
    ClusterMember(NodeID t):nodeid_(t){}

    //查看是否健康
    ResCode GetClusterSize();
private:
    NodeID nodeid_;
};

class CluseterConfig: public ClusterMember {
public:
    CluseterConfig(NodeID t);

    //设置集群大小
    void SetSize(uint32_t s);

    //加载配置从配置文件读取，l5，namespace
    void ReadNodeInfoFromConf();

private:
    std::vector<ClusterMember *> members_; //本地的指向本地
    NodeSize size_;
    NodeID localNodeId_;
};

};

#endif