#ifndef __EPAXOS_LEVELDB_DB_H__
#define __EPAXOS_LEVELDB_DB_H__

#include <leveldb/db.h>
#include "include/storage_interface.h"
#include "include/LRUCache11.h"

namespace epaxos{

class LeveldbStorageKv :public StorageBaseInterface{
public:
    LeveldbStorageKv(std::string filpath);
    ~LeveldbStorageKv(){delete db_;}

    ResCode read(const std::string &key,std::string &value);

    ResCode write(const std::string &key,const std::string &value,bool sync=false);

    ResCode batchread(std::unordered_map<std::string,std::string>& mp);
    ResCode batchwrite(std::unordered_map<std::string,std::string>& mp,bool sync=false);
    
private:
    leveldb::DB*  db_;
};

//使用lrucache
template<typename T>
class ProtobufCacheHandler{
public:
    ProtobufCacheHandler(StorageBaseInterface * db):db_interface_(db){
        cache_ = new lru11::Cache<std::string,T>(10,2);
        assert(cache_!=nullptr);
    }
    ~ProtobufCacheHandler(){
        delete cache_;
    }

    ResCode Get(const std::string &key, T &value){
         //先查cache 
        bool exist=cache_->tryGet(key,value);
        if(exist){
            return ResCode::Success();
        }
        spdlog::trace("ProtobufCacheHandler::get local cache miss key:{}",key);
        //假如不存在
        
        ResCode r = ReadOneFromDb(key,value);
        if(r.IsError()){
            spdlog::error("ProtobufCacheHandler::ReadOneFromDb failed key:{}",key);
            return r;
        }
    
        return ResCode::Success();
    }

    ResCode Set(const std::string &key, T &value);

    ResCode BatchSet(std::unordered_map<std::string,T>& mp){
        std::unordered_map<std::string,std::string> readyToWrite;
        for(auto iter = mp.begin();iter!=mp.end();iter++){
            std::string value;
            if (iter->second.SerializeToString(&value)){
                return ResCode::EncodeErr();
            }
            readyToWrite[iter->first]=value;
            cache_->insert(iter->first,iter->second);
        }
        ResCode r= db_interface_->batchwrite(readyToWrite);

        return r;
    }

    ResCode ReadOneFromDb(const std::string &key, T&value){
        std::string strv;
        ResCode r = db_interface_->read(key,strv);
        if(r.IsError()){
            spdlog::error("ProtobufCacheHandler::ReadOneFromDb db read failed res:{}",r.GetRemote());
            return r;
        }
        bool succ = value.ParseFromString(strv);
        if(!succ){
            spdlog::error("ProtobufCacheHandler::ReadOneFromDb parse from string failed key:{}",key);
            return ResCode::DecodeErr();
        }
        cache_->insert(key,value);
        return ResCode::Success();
    }

protected:
    lru11::Cache<std::string,T> *cache_;

    StorageBaseInterface *db_interface_;//如果不存在就直接读取
};

}

#endif 

