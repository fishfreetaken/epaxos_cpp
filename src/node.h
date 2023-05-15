
#ifndef __EPAXOS_NODE_INST_
#define __EPAXOS_NODE_INST_
#include "comminclude.h"

namespace epaoxs {

class Node {
public:
    Node(uint64_t t):val_(t){}

    uint32_t GetVale(){return val_;}
private:
    //依赖的值
    uint64_t val_;

    std::vector<uint64_t> deps_;
};


}

#endif

