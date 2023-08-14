
#ifndef _EPAXOS_NODE_NUM_CFG_H__
#define _EPAXOS_NODE_NUM_CFG_H__
#include "include/comminclude.h"
#include <fstream>
#include <sys/stat.h>
#include "proto/instance.pb.h"
/***
 * 
 * 
 * 
 * 
 * ***/
namespace epaxos{

//uint64_t
template<typename T>
class NodeNumCfg{
public:
    NodeNumCfg(std::string filename ,size_t offset=0):file_len_(0),step_(sizeof(T)),begin_offset_(offset){
        fs_.open(filename,std::ios::out | std::ios::in |std::ios::binary);
        if (!fs_.is_open()){
            fs_.close();
            //创建文件
            std::ofstream of(filename,std::ios::out|std::ios::binary);
            of.close();
            fs_.open(filename,std::ios::out | std::ios::in |std::ios::binary);
            assert(fs_.is_open());
        }
        struct stat statbuf;
        stat(filename.c_str(), &statbuf);
        file_len_ = statbuf.st_size;
        spdlog::trace("open file file:{} size:{}",filename,file_len_);
        fs_.seekg(0L,std::ios::beg);
    
    }

    ~NodeNumCfg(){
        fs_.close();
    }

    T readins(){
        size_t seek_len = (begin_offset_+1)*step_;
        if(file_len_< seek_len ){
            spdlog::error("seeklen:{} < filele:{}",seek_len,file_len_);
            return 0;
        }
       fs_.seekg(begin_offset_*step_,std::ios::beg);

        T r = 0;
       fs_.read(reinterpret_cast<char*>(&r),step_);
        return r;
    }

    void writeins(T t){
        size_t seek_len = (begin_offset_+1)*step_;
        if(file_len_ < seek_len){
            //填充0
            size_t n = (seek_len - file_len_)/step_;
           fs_.seekg(0, std::ios::end);
            fillZero(n);
            file_len_ = seek_len;
        }
       fs_.seekg(begin_offset_*step_,std::ios::beg);
       fs_.write(reinterpret_cast<char*>(&t),step_);
    }

    void writepb(const T &t){
        std::string value;
        t.SerializeToString(&value);
        fs_.seekg(begin_offset_,std::ios::beg);
        uint64_t sz = value.size();
        fs_.write(reinterpret_cast<char*>(&sz),sizeof(uint64_t));

        fs_.write(value.c_str(),value.size());

        spdlog::trace("writepb size:{} value:{}",sz,value.size());
    }

    void readpb( T &t){
        fs_.seekg(begin_offset_,std::ios::beg);
    
        uint64_t sz =0;
        fs_.read(reinterpret_cast<char*>(&sz),sizeof(uint64_t));

        assert(sz>0);

        std::string value(sz,'\0');
        fs_.read(&value[0],sz);

        spdlog::trace("writepb size:{} value:{}",sz,value.size());
        assert(t.ParseFromString(value));
    }

private:
    void fillZero(uint32_t n){
        T r = 0;
        for(size_t i=0;i<n;i++){
           fs_.write(reinterpret_cast<char*>(&r),step_);
        }
    }

private:
    std::fstream fs_; 

    size_t file_len_;

    size_t step_;

    size_t begin_offset_;
};

class NodeLocalInstanceId{
public:
    NodeLocalInstanceId(){
        mt_= new NodeNumCfg<uint64_t>("test.txt");
        cf_ = new NodeNumCfg<epxos_instance_proto::EpGlobalConf>("conf.db");
    }

    ~NodeLocalInstanceId(){
        delete mt_;
        delete cf_;
        mt_=nullptr;
        cf_ = nullptr;

        delete inner_db_;
        delete public_db_;
        public_db_=nullptr;
        inner_db_ =nullptr;
    }

    //自增本地的insid并返回
    uint64_t IncId(){
        uint64_t ins=  mt_->readins();
        ins++;
        mt_->writeins(ins);
        return ins;
    }

    uint64_t GetInsId(){
        return mt_->readins();
    }

    void Init(){
        cf_->readpb(ecf_);
        inner_db_file_ = ecf_.inner_db_file();
        if(inner_db_file_.size()==0){
            inner_db_file_ = "./innerdb";
        }
        inner_db_ = new epaxos::LeveldbStorageKv(inner_db_file_);

        public_db_file_ = ecf_.public_db_file();
        if(public_db_file_.size()==0){
            public_db_file_ = "./testdb";
        }
        public_db_ = new epaxos::LeveldbStorageKv(public_db_file_);

        assert(public_db_ != nullptr && inner_db_!=nullptr);
    }

    void UpdatelocalCf(const epxos_instance_proto::EpGlobalConf & t){
        ecf_.Clear();
        ecf_.CopyFrom(t);
        cf_->writepb(t);
    }

    const epxos_instance_proto::EpGlobalConf & GetLocalCf(){return ecf_;}

    epaxos::LeveldbStorageKv* GetInnerDb(){return inner_db_;}
    epaxos::LeveldbStorageKv* GetPublicDb(){return public_db_;}

private:
    NodeNumCfg<uint64_t> *mt_;

    epxos_instance_proto::EpGlobalConf ecf_; //将本地配置保存在本地，允许通过json的方式进行修改

    NodeNumCfg<epxos_instance_proto::EpGlobalConf> * cf_;

    std::string inner_db_file_;
    std::string public_db_file_;

    epaxos::LeveldbStorageKv *inner_db_;
    epaxos::LeveldbStorageKv *public_db_;
};

};
#endif 