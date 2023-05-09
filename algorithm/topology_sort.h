

#include "include/node.h"

/// @brief 拓扑排序
namespace epaxos{

class topologysort{
public:
    topologysort(Node *root):root_(root){}
    void Sort();

public:
    void AddNode(Node*t){
        
    }
    //返回一个排序后的
    void DoSort(std::vector<Node*> &res);
private:
    std::vector<Node*> m_vctBeginNodes;
    std::unordered_map<uint64_t,std::vector<uint64_t> > m_dict;
    std::unordered_map<uint64_t,uint32_t> m_score;
};

}
