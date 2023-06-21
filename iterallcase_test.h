
#ifndef __EPAXOS_TEST_INTERALLCASES__

#define __EPAXOS_TEST_INTERALLCASES__
#include "src/instance.h"

#include "util.h"
namespace epaxos_test {

class Case_Node{
public:
    Case_Node(){}
    Case_Node(uint32_t t):id_(t){}
    Case_Node(uint32_t t,uint32_t seg):id_(t),segrate_level_(seg){
        assert(t<=seg);
    }

    bool IsSegtive(const Case_Node&a)const { //这个值是否被本身隔离
        if(segrate_level_ == 0 ){
            return false;
        }
        return segrate_level_ <= a.id_;
    }
    uint32_t Value()const {return id_;}
private:
    uint32_t id_;
    uint32_t segrate_level_;    //不能超过这个值
};

template<typename T>
class Iter_all_cases{
public:
    Iter_all_cases(uint32_t size):len_(size),pos_(0){
        mts_.resize(len_);
        std::iota(mts_.begin(),mts_.end(),0);
    }
    Iter_all_cases(const std::vector<T> &dt):len_(dt.size()),pos_(0){
        std::copy(dt.begin(),dt.end(), std::back_inserter(mts_));
        std::cout<< PrintfVectorN<T>(mts_) <<std::endl;
    }
    size_t Init(){
       backtrack(mts_,0);
       return res_.size();
    }
    void Reset(){
        pos_ = 0;
    }
    std::vector<T> * GetOneCase(){
        if(res_.size()==pos_){
            return nullptr;
        }
        return &res_[pos_++];
    }
private:
    void backtrack(std::vector<T>& input, uint32_t first){
        // 所有数都填完了
        if (first == len_) {
            res_.emplace_back(input);
            return;
        }
        for (uint32_t i = first; i < len_; ++i) {
            // 动态维护数组
            if(input[first].IsSegtive(input[i])){
                //判断是否是非法数组
                continue;
            }
            std::swap(input[i], input[first]);
            // 继续递归填下一个数
            backtrack( input, first + 1);
            // 撤销操作
            std::swap(input[i], input[first]);
        }
    }
private:
    std::vector<std::vector<T>> res_;
    std::vector<T> mts_;

    uint32_t len_;
    uint32_t pos_;
};

};

#endif
