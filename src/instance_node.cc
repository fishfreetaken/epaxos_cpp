#include "instance_node.h"
#include "proto/instance.pb.h"
namespace epaxos{

bool Instance::step(epxos_instance_proto::InstanceSwapMsg &ins){
    bool sync=false;
    //每次都需要更新key和instance
    if(ins.insc().ballot() != ins_.ballot()){
        //todo
        spdlog::warn("ballot not equal from:{} local:{}",ins.insc().DebugString(),ins_.DebugString());
        assert(false);
    }
    if(ins.insc().state() < ins_.state()){
        spdlog::warn("behind the message");
        assert(false);
    }
    switch(ins.insc().state()){
        case epxos_instance_proto::EPXOS_EM_WK_PREACCEPT:
            //加载本地所有key更新过去
            break;
        case epxos_instance_proto::EPXOS_EM_WK_ACCEPT:
            break;
        case epxos_instance_proto::EPXOS_EM_WK_COMMIT:
            //不需要更新key，直接更新ins
            break;
        default:
            //null;
            break;
    }
    return sync;
}

};