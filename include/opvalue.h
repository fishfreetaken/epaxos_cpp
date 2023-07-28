
#ifndef __EPAXOS_H_OP_VALUE_ 
#define __EPAXOS_H_OP_VALUE_ 

#include "storage_tranfer.h"

namespace epaxos{

enum emOperationType{
    SET = 1,
    DEL = 2,
    GET = 3,
    CGET = 4,
    CSET = 5,
};

class ValueVersion :public IndexItem{
public:
    ValueVersion():IndexItem(0){}
};

class OpValue : public StMemoryItem{
public:
    OpValue(std::string &s):vbody_(s),op_(SET){}
    OpValue():op_(SET){}
public:
    std::string GetKey()const ;
    ResCode Encode(std::string &a)const;
    ResCode Decode(const std::string & v); 
private:
    emOperationType op_;
    ValueVersion version_;
    std::string oldvalue_;
    std::string vbody_;     //
};

}

#endif 