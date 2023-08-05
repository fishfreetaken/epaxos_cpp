
#ifndef __EXPXOS_H_STORAGE_INTERFACE__ 
#define __EXPXOS_H_STORAGE_INTERFACE__

#include <string>
#include <unordered_map>
#include "rescode.h"

namespace epaxos{
class StorageBaseInterface{
public:
    virtual ResCode read(const std::string &key,std::string &value)=0;
    virtual ResCode write(const std::string &key,const std::string &value,bool sync=false)=0;

    virtual ResCode batchread(std::unordered_map<std::string,std::string>& mp)= 0;
    virtual ResCode batchwrite(std::unordered_map<std::string,std::string>& mp,bool sync=false)= 0;
};

};
#endif

