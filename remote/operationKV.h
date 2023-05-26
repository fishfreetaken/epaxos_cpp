#ifndef __EPAXOS_CLIENT_OP_KV__
#define __EPAXOS_CLIENT_OP_KV__
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "../include/rescode.h"

namespace epaxos_client{

enum emOperationType{
    SET = 1,
    DEL = 2,
    GET = 3,
    CGET = 4,
    CSET = 5,
};

class OperationKV{
public:
    OperationKV(std::string value):value_(value),op_(SET),version_(0){}
    OperationKV(std::string value,emOperationType op):value_(value),op_(op),version_(0){}

    void SetOpToDel(){op_=DEL;}
    void SetOpToGet(){op_=GET;}

    void SetCOpToGet(uint32_t v){op_=CGET; version_= v;}
    void SetCOpToSet(uint32_t v){op_=CSET; version_= v;}

private:
    std::string value_;
    emOperationType op_;
    uint32_t version_;
};

class OperationKVArray {
public:
    epaxos::ResCode InsertOne(std::string key, OperationKV & one);
    int GetArrKeys(std::vector<std::string> &keys) const;
private:
    std::unordered_map<std::string,std::vector<OperationKV> > arr_;

    std::string uniqid_;
};
};



#endif 