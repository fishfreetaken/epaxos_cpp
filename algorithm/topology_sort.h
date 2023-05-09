

#include "include/node.h"

/// @brief 拓扑排序
namespace epaxos{

class topologysort{
public:
    topologysort(Node *root):root_(root){}
    void Sort();

    void GetResult();

private:
    Node * root_;
    
    
};

}
