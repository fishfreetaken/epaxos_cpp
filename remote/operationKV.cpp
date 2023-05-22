

#include "operationKV.h"

namespace epaxos_client{

int OperationKVArray::GetArrKeys(std::vector<std::string> &keys)const {
    std::for_each(arr_.begin(),arr_.end(),[&](const std::pair <std::string,std::vector<OperationKV> >  & value) {
        keys.push_back(value.first);
    });
    return keys.size();
}

ResCode OperationKVArray::InsertOne(std::string key,OperationKV & one){
    arr_[key].push_back(one);
    return ResCode(2);
}

};