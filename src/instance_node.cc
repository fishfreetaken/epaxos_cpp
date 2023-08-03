#include "instance_node.h"

namespace epaxos{

ResCode Instance::InitInsFromString(std::string &body){
    std::cout<<"before: "<< ins_.DebugString()<<std::endl;
    bool t = ins_.ParseFromString(body);
    std::cout<<"localind: "<< ins_.DebugString()<<std::endl;
    return ResCode(t);
}

};