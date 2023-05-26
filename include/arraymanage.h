
#ifndef __EPAXOS_ARRAYMANAGE__
#define __EPAXOS_ARRAYMANAGE__
#include "comminclude.h"
#include "nodecfg.h"
namespace epaxos {

class ArrayItem {
public:
    virtual IfChange DoUpdate(const ArrayItem & t);
};

/**
 * @brief 初始化更新之后就不能发生改变
 * 
 */
class IndexValue {
public:
    IndexValue() = delete;
    IndexValue(uint64_t t):val_(t){}

    uint64_t Value64(){return val_;}

    bool operator ==(const IndexValue &a)const { return a.val_ == val_;}
private:
    uint64_t val_;
};

/**
 * @brief 只能通过比较进行更新
 * 
 */
class IndexItem : public ArrayItem{
public:
    IndexItem():idx_(0){}
    IndexItem(uint64_t t):idx_(t){}

    uint64_t Value64() const {return idx_;}

    IndexItem Inc(){return IndexItem(++idx_);}

    IfChange RefeshLocToAhead(const IndexItem & a){ bool res = a.idx_ >= idx_; idx_ = a.idx_ >= idx_ ? a.idx_+1 : idx_; return IfChange(res); }
    IfChange DoUpdate(const IndexItem & a){ bool res = a.idx_ >= idx_; idx_ = a.idx_ >= idx_ ? a.idx_ : idx_; return IfChange(res); }

    std::string GetString(){return std::to_string(idx_);}

    bool operator < (const IndexItem &a )const{return idx_ < a.idx_;}

private:
    uint64_t idx_;
};

class ArrayManage{
public:
    ArrayManage(IndexValue &size):vec_(size.Value64()){}
    ArrayManage(const ArrayManage &a) {
        vec_.clear();
        std::copy(a.vec_.begin(),a.vec_.end(), std::back_inserter(vec_));
    }

    IfChange UpdateOne(IndexValue & idx, const ArrayItem* y){
        assert(idx.Value64() < vec_.size() );
        return vec_.at(idx.Value64())->DoUpdate(*y);
    }
    IfChange UpdateArray(const ArrayManage & t){
        assert(vec_.size()==t.vec_.size());
        IfChange change;
        for(size_t i=0;i<vec_.size();i++ ){
            IfChange ttmp =  vec_.at(i)->DoUpdate(*t.vec_.at(i));
            change.Refresh(ttmp);
        }
        return change;
    }

private:
    std::vector<ArrayItem*> vec_;
};

};

#endif