
#ifndef __EPAXOS_KEY_COLLECTOR__ 
#define __EPAXOS_KEY_COLLECTOR__
#include "instance.h"
#include <unordered_map>
namespace epaxos {
    
class KeyUnit {

private:
    InstanceID id_;
    std::string value_;     //可以是json，value，或者二进制协议,这里替换一下string
};

class KeyUnitMsgBody: public KeyUnit{

private:
    std::string key_;
};


class KeyVersionArray {
public:
    
    /** 
     * 
     * 
     * 获取这个key的版本的最大的insid值
     */
    InstanceID GetMaxInstanceID();

    void Evict();   //淘汰清理掉一些存储
    ResCode Push(const std::string &v,const InstanceID & id); //满了，值已经存在了，成功 ，要判断大于前一个instanceidS
    ResCode Push(const InstanceID & id); //放入一个空的instancid
    ResCode Push(KeyUnit &u);

    KeyUnit* GetLastOne(); //
    bool IsEmpty(){return list_.size()==0;}

public:
    std::string key_;
    std::vector<KeyUnit*> list_;    //智能指针
};

class KeyCollector{
public:
    void Insert(KeyUnitSwap & u);
    void GetInterfaceSeq(KeyVersionArray&keyArray);
private:
    std::unordered_map<std::string,KeyVersionArray*> dict_;
};

};

#endif