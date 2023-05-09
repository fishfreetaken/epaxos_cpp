
#ifndef __EPAXOS_COMMUNIT_PROTO__
#define __EPAXOS_COMMUNIT_PROTO__

#include "instance.h"
namespace epaxos {

class MsgSeqId public: SeqId {
public :
    MsgSeqId(InstanceNode *t);
}

class KeyUnitMsgBody: public KeyUnit{

private:
    std::string key_;
};

/**
 * 主协议的交换流程 
*/
class EProcessMsgBody{

private:
    MsgSeqId seqId_;
    KeyUnitMsgBody msgBody_;
};

};


#endif