
#ifndef __EPAXOS_ARRAYMANAGE__
#define __EPAXOS_ARRAYMANAGE__
#include "rescode.h"
#include "nodecfg.h"

namespace epaxos {

/**
 * @brief 只能通过比较进行更新
 * 
 */
class BaseValue {
public:
    BaseValue():idx_(0){}
    BaseValue(uint64_t t):idx_(t){}

    uint64_t Value64() const {return idx_;}

    BaseValue Inc(){return BaseValue(++idx_);}

    //增加自己就好了，为什么要增加别人的 
    IfChange RefeshLocToAhead(const BaseValue & a){ bool res = a.idx_ >= idx_; idx_ = a.idx_ >= idx_ ? a.idx_+1 : idx_; return IfChange(res); }
    IfChange DoUpdate(const BaseValue & a){ bool res = a.idx_ > idx_; idx_ = a.idx_ > idx_ ? a.idx_ : idx_; return IfChange(res); }

    bool operator < (const BaseValue &a )const{return idx_ < a.idx_;}
    bool operator == (const BaseValue &a )const{return idx_ == a.idx_;}

    bool IsNull(){return idx_==0;}

private:
    uint64_t idx_;
};

//自增的seqid
class MutexSeqID :public BaseValue{
public:
    MutexSeqID(BaseValue a):BaseValue(a){}
};

//只能单调递增
class InsID : public MutexSeqID{
public:
    InsID(MutexSeqID seq,BaseValue insId,NodeID n):MutexSeqID(seq),insid_(seq),nid_(n){}
    IfChange UpdateSeq(MutexSeqID t){ return DoUpdate(t);}
private:
    BaseValue insid_;
    NodeID nid_;
};

class BallotID : public BaseValue{
public :
};

};

#endif