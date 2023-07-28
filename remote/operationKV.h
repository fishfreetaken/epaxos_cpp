#ifndef __EPAXOS_CLIENT_OP_KV__
#define __EPAXOS_CLIENT_OP_KV__
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "../include/rescode.h"

#include "../include/storage_tranfer.h"
#include "../include/opvalue.h"

namespace epaxos_client{

class OperationKV: public epaxos::StMemoryItem  {
public:
    OperationKV(std::string value):value_(value),op_(SET),version_(0){}

    std::string GetKey()const {return key_;} ;
    epaxos::ResCode Encode(std::string &a)const {return epaxos::ResCode(0);} 
    epaxos::ResCode Decode(const std::string & v) { return epaxos::ResCode(0); } 

private:
    std::string key_;
    epaxos::OpValue value_;
    
    uint32_t version_;
};

class OperationKVArray : public epaxos::StMemoryItemsArry {
public:
    epaxos::ResCode InsertOne(std::string key, OperationKV & one);

    int GetArrKeys(std::vector<std::string> &keys) const;

private:
    std::unordered_map<std::string,std::vector<OperationKV> > arr_;

    std::string uniqid_;
};


};



#endif 