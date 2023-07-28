#ifndef __EPAXOS_STORAGE_TRANFER_H_

#define __EPAXOS_STORAGE_TRANFER_H_

#include "storage_interface.h"
#include "rescode.h"
namespace epaxos {

class StMemoryItemsArry{
public: 
    //const
    virtual void GetKeys(std::map<std::string,std::string> & mk)const = 0;

    virtual  ResCode BatchDecode(std::map<std::string,std::string> & mk) = 0;

    virtual ResCode BatchEncode(std::map<std::string,std::string> &mk)const = 0;
};

/// @brief 素材数据的转换 
template<typename T>
class StorageMsgTransfer{
public: 
    StorageMsgTransfer(StorageBaseInterface *db):db_interface_(db){}

    virtual ResCode write(std::string &key,const T&t){
       // std::string key = t.GetKey();
        std::string value;
        t.SerializeToString(&value);
        return db_interface_->write(key,value);
    }

    virtual ResCode read(std::string &key, T&t){
       // std::string key = t.GetKey();
        std::string value ;
        db_interface_->read(key,value);
        return t.ParseFromString(value);
    }

    virtual ResCode BatchRead(StMemoryItemsArry* t){
        std::map<std::string ,std::string> ms;
        t->GetKeys(ms);
        db_interface_->batchread(ms);
        return  t->BatchDecode(ms);
    }

    virtual ResCode BatchWrite(const StMemoryItemsArry* t ){
        std::map<std::string ,std::string> ms;
        t->BatchEncode(ms);
        return db_interface_->batchwrite(ms);
    }

private:
    StorageBaseInterface *db_interface_;
}; 

    
};
#endif 