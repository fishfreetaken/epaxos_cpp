
#include "leveldb_kv.h"
#include "leveldb/write_batch.h"
namespace epaxos{

LeveldbStorageKv::LeveldbStorageKv(std::string filpath){
    leveldb::Options options;
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, filpath, &db_);
    assert(status.ok());
}

ResCode LeveldbStorageKv::batchread(std::unordered_map<std::string,std::string>& mp){
    // level
    assert(false);
    return ResCode::Success();;
}

ResCode LeveldbStorageKv::read(const std::string &key,std::string &value){
    leveldb::Status s = db_->Get(leveldb::ReadOptions(),key,&value);
    if (!s.ok()){
        if (s.IsNotFound()){
            return ResCode::NotFound();
        }
        return ResCode::DBReadErr();
    }
    return ResCode::Success();
}

ResCode LeveldbStorageKv::write(const std::string &key,const std::string &value,bool sync){
    leveldb::Status s = db_->Put(leveldb::WriteOptions(),key,value);
    if (!s.ok()){
        return ResCode::DBWriteErr();
    }
    return ResCode::Success();
}

ResCode LeveldbStorageKv::batchwrite(std::unordered_map<std::string,std::string>& mp,bool sync){
    leveldb::WriteBatch batch;
    for(auto iter=mp.begin();iter!=mp.end();iter++){
        batch.Put(iter->first,iter->second);
    }
    leveldb::Status s = db_->Write(leveldb::WriteOptions(),&batch);
    if (!s.ok()){
        return ResCode::DBWriteErr();
    }
    return ResCode::Success();
}

void LeveldbStorageKv::del(std::string key){
    db_->Delete(leveldb::WriteOptions(),key);
}

/*
template<typename T>
ResCode ProtobufCacheHandler<T>::Get(const std::string &key, T &value ){
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

template<typename T>
ResCode ProtobufCacheHandler<T>::ReadOneFromDb(const std::string &key, T&value){
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


//原子性
template<typename T>
ResCode ProtobufCacheHandler<T>::Set(const std::string &key, T &value){
    return ResCode::Success();
}

template<typename T>
ResCode ProtobufCacheHandler<T>::BatchSet(std::unordered_map<std::string,T>& mp ){
    std::unordered_map<std::string,std::string> readyToWrite;
    for(auto iter = mp.begin();iter!=mp.end();iter++){
        std::string value;
        if (iter->SerializeToString(&value)){
            return ResCode::EncodeErr();
        }
        readyToWrite[iter->first]=value;
        cache_->insert(iter->first,value);
    }
    ResCode r= db_interface_->batchwrite(readyToWrite);

    return r;
}
*/

};
