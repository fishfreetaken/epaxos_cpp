
#ifndef __EPAXOS_KV_H_STORE__

#define __EPAXOS_KV_H_STORE__
#include "instance.h"
#include "../include/opvalue.h"

namespace epaxos
{

class KvEpaxosValue :public StMemoryItem{
public:
    std::string GetKey()const ;
    ResCode Encode(std::string &a)const ;
    ResCode Decode(const std::string & v); 
private:
    InsID lastIns_;       //最后一个insid

    std::string key_;
    OpValue value_;     //decode
};

class KvEpaxosValueArray : public StMemoryItemsArry<KvEpaxosValue> {
public:
    //一次性语意，必须更新插入
    ResCode Insert(const KvEpaxosValue &t);

    ResCode BatchDecode(std::map<std::string,std::string> & mk);

    //const
    void GetKv(std::map<std::string,std::string> & mk)const;
};


} // namespace name


#endif