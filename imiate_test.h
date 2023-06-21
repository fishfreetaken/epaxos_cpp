
#ifndef __EPAXOS_TEST_IMIATE_TEST_H__
#define __EPAXOS_TEST_IMIATE_TEST_H__

#include "src/instance.h"

std::vector<epaxos::InstanceNode *> gIns;

namespace epaxos_test {
class WorkNodeBase {
public:
    uint32_t GetNodeId(){return nid_;}
protected:
    enum emWorkNodeType{    
        EM_PROMOTE_MSG = 1,
        EM_RECEIVE_MSG = 2,
    };

    WorkNodeBase(uint32_t n , emWorkNodeType tp):nid_(n),type_(tp){}
    uint32_t nid_;
    emWorkNodeType type_;
};

class WorkNode :public WorkNodeBase {
public:
    WorkNode(uint32_t t):WorkNodeBase(t,EM_PROMOTE_MSG),ts_(nullptr){}
    ~WorkNode(){
        if(ts_!=nullptr){
            delete ts_;
        }
    }
    epaxos::InstanceSwap GetSwapMsg(){
        if(ts_==nullptr) {
            epaxos_client::OperationKVArray arrvalues;
            ts_ = new epaxos::InstanceSwap(gIns[GetNodeId()]->GenNewInstance(arrvalues));
        }
        return ts_->New();
    }

    void Reset(){
        if(ts_!=nullptr){
            delete ts_;
            ts_=nullptr;
        }
    }
private:
    epaxos::InstanceSwap *ts_;
};

class ReceiveNode :public WorkNodeBase {
public:
    ReceiveNode(uint32_t t):WorkNodeBase(t,EM_RECEIVE_MSG){}
};

//生成一对
class WorkPair{
public:
    WorkPair(WorkNode *a, uint32_t b):w_(a),c_(b){}
    epaxos::InstanceSwap SwapMsg(){
       epaxos::InstanceSwap sp = w_->GetSwapMsg();

       std::cout << "[writeNode:"<< w_->GetNodeId()<<"]"<<"[readnode:"<< c_.GetNodeId()<<"]";
       if(c_.GetNodeId() == w_->GetNodeId()){
            return  sp;
       }
       gIns[c_.GetNodeId()]->MutualManageIns(sp);
       gIns[w_->GetNodeId()]->ReFreshLocal(sp);
    
       std::cout<<"writeNode detail:" << gIns[w_->GetNodeId()]->DebugPrintInstanceNode() << "res swap:";
       return sp;
    }

    std::string GetNodeInfo(){
        std::stringstream sst;
        sst<<"[WorkNode:"<<w_->GetNodeId() <<" ReceiveNode:" << c_.GetNodeId() <<"]";
        return sst.str();
    }
private:
    WorkNode *w_;
    ReceiveNode c_;
};

class WorkManager {
public:
    WorkManager(uint32_t writeNum):wr_(writeNum,0){
        std::iota(wr_.begin(),wr_.end(),0);
    }
    /**
     * @brief 选择其中两个以上的writer，进行交换信息，选择三个case进行接受信息
     * 
     * @param selectwrite 
     * @param suballcases 
     */
    uint32_t Init(std::vector<uint32_t> &writer, std::vector<uint32_t> & recivers){ //生产者的数量
        // selectwrite.size() + suballcases.size()
        for(size_t i = 0;i<writer.size();i++){
            wr_.push_back(WorkNode(recivers[i]));
            vec_.push_back(WorkPair(&wr_.back(),recivers[i]));
        }
        //将vec进行打乱来得到一个全排列
        return vec_.size();
    }

    epaxos::InstanceSwap StepAt(size_t i ){
        return  vec_[i].SwapMsg();
    }

    void Reset(){
        std::for_each(wr_.begin(),wr_.end(),[](WorkNode &a){
            a.Reset();
            return ;
        });
    }
private:
    std::vector<WorkPair> vec_;
    std::vector<WorkNode> wr_;
};

};

#endif