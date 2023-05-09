
#ifndef __EPAXOS_COMMUNIT_PROTO__
#define __EPAXOS_COMMUNIT_PROTO__

#include "instance.h"
namespace epaxos {
class SeqId{
public:
    SeqId(InstanceNode *i);
private:
    uint64_t seq_;
};

//交换协议
class Communit{

};

};


#endif