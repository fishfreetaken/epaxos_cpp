
#ifndef __EPAXOS_TEST_INTERALLCASES__

#define __EPAXOS_TEST_INTERALLCASES__
#include "src/instance.h"

#include "util.h"
#include <set>
namespace epaxos_test {

class Case_Node{
public:
    Case_Node(){}
    Case_Node(uint32_t t):id_(t),segrate_up_level_(0),segrate_low_level_(0){}
    Case_Node(uint32_t t,uint32_t seg):id_(t),segrate_up_level_(seg),segrate_low_level_(0){
        assert(t<=seg);
    }

    Case_Node(uint32_t t,uint32_t low,uint32_t up):id_(t),segrate_up_level_(up),segrate_low_level_(low){
        assert(t<=segrate_up_level_ || t >= segrate_low_level_);
    }

    bool IsSegtive(const Case_Node&a) const{
        if(segrate_up_level_ != 0 && segrate_up_level_ <= a.id_ ){
            return true;
        }
        if(segrate_low_level_ != 0 && segrate_low_level_ >= a.id_ ){ //不能低于这个值
            return true;
        }
        return false;
    }
    uint32_t Value()const {return id_;}
private:
    uint32_t id_;
    uint32_t segrate_up_level_;    //不能超过这个值
    uint32_t segrate_low_level_; //不能越过这个值
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
    }
    //遍历带隔离保护的case
    size_t InitSeg(){
       Reset();
       backtrackseg(0);
       return res_.size();
    }
    //遍历出所有case
    size_t InitAll(){
        Reset();
        backtrack(0);
        return res_.size();
    }
    void Reset(){
        res_.clear();
        pos_ = 0;
    }
    std::vector<T> * GetOneCase(){
        if(res_.size()==pos_){
            return nullptr;
        }
        return &res_[pos_++];
    }
private:
    void backtrackseg(uint32_t first){
        // 所有数都填完了
        if (first == len_) {
            res_.emplace_back(mts_);
            return;
        }
        for (uint32_t i = first; i < len_; ++i) {
            // 动态维护数组
            if(mts_[first].IsSegtive(mts_[i])){
                //判断是否是越界
                continue;
            }
            //用被交换的遍历一下之前的值
            bool bFilter = false;
            for(uint32_t j=first;j<i;j++){
                 if(mts_[j].IsSegtive(mts_[i])){
                    //判断是否是越界
                    bFilter = true;
                    break;
                }
            }
            if(bFilter){
                continue;
            }
            std::swap(mts_[i], mts_[first]);
            // 继续递归填下一个数
            backtrackseg(  first + 1);
            // 撤销操作
            std::swap(mts_[i], mts_[first]);
        }
    }

    void backtrack(uint32_t first){
        // 所有数都填完了
        if (first == len_) {
            res_.emplace_back(mts_);
            return;
        }
        for (uint32_t i = first; i < len_; ++i) {
            // 动态维护数组
            std::swap(mts_[i], mts_[first]);
            // 继续递归填下一个数
            backtrack( first + 1);
            // 撤销操作
            std::swap(mts_[i], mts_[first]);
        }
    }
private:
    std::vector<std::vector<T>> res_;
    std::vector<T> mts_;

    uint32_t len_;
    uint32_t pos_;
};

class IterAllCase{
public:
  IterAllCase(uint32_t baseNum,uint32_t caseNum):pos_(0),count_(baseNum),div_(caseNum){
    //获取全排列
    Init();
  }
  uint32_t GetOneCase(std::vector<uint32_t> &mt){
    if(pos_ >= vc_.size()){
      reset();
      return 0;
    }
    mt.clear();
    uint32_t t = vc_[pos_++];
    uint32_t s=0;
    while(t>0){
      if( (t&1) >0 ){
        mt.push_back(s);
      }
      s++;
      t=t>>1;
    }
    return mt.size();
  }
  void reset(){
    pos_=0;
  }
private:
  
  void dfs(std::set<int> &ans,uint32_t level,uint32_t begin, uint32_t val){
    //std::cout<<level<<" "<<begin<< " "<< val<<std::endl;
    if(level==0){
      ans.insert(val);
      return ; 
    }

    level--;
    for(size_t i=begin;i<count_;i++){
      val= val | (1<<i);
      dfs(ans,level,i+1,val);
      val = val & (~(1<<i));
    }
  }
private:
  void Init(){
    std::set<int> st;
    dfs(st,div_,0,0);
    for(auto it=st.begin();it!=st.end();it++){
      vc_.push_back(*it);
    }
    std::cout<<"last total count "<<vc_.size()<<std::endl;
  }
private:
  std::vector<uint32_t> vc_;
  uint32_t pos_;
  uint32_t count_;
  uint32_t div_;
};



};

#endif
