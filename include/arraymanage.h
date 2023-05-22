
#ifndef __EPAXOS_ARRAYMANAGE__
#define __EPAXOS_ARRAYMANAGE__
#include "comminclude.h"
namespace epaxos {

class Index{
public:
    Index():idx_(0){}
    Index(uint64_t t):idx_(t){}

    uint64_t Value() const {return idx_;}

    uint64_t Inc(){return ++idx_;}

    bool UpdateAhead(const Index & a){ bool res = a.idx_ >= idx_; idx_ = a.idx_ >= idx_ ? a.idx_+1 : idx_; return res; }
    bool Update(const Index & a){ bool res = a.idx_ >= idx_; idx_ = a.idx_ >= idx_ ? a.idx_ : idx_; return res;}

    std::string GetString(){return std::to_string(idx_);}

    bool operator < (const Index&a)const;

private:
    uint64_t idx_;
};

template<typename T>
class ArrayManage{
public:
    ArrayManage(Index size):vec_(size.Value()){}

    ArrayManage()

    T* at(Index i) { assert(i.Value()<vec_.size()); return &vec_[i.Value()];}
private:
    std::vector<T> vec_;
    Index local_;
};

};

#endif