
#ifndef __EPAXOS_TEST_IMIATE_TEST_H__
#define __EPAXOS_TEST_IMIATE_TEST_H__

#include "src/instance.h"
#include "iterallcase_test.h"

std::vector<epaxos::InstanceNode *> gIns;

namespace epaxos_test {
class WorkNodeBase {
public:
    uint32_t GetNodeId() const {return nid_;}
    uint32_t Value() const {return nid_;}
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
       if(c_.GetNodeId() == w_->GetNodeId()){
            return sp;
       }
       gIns[c_.GetNodeId()]->PreAccept(sp);
       gIns[w_->GetNodeId()]->ReFreshLocal(sp);

       //std::cout<< w_->GetNodeId() << c_.GetNodeId() << gIns[w_->GetNodeId()]->DebugPrintInstanceNode()<<"[swap:]"<<sp.GetDetailInfo()<<std::endl;
       return sp;
    }

    std::string Value() const{
        std::stringstream sst;
        sst << "[workNode:"<<w_->Value() << "]" <<"[reciveNode:"<<c_.Value()<<"]"<<std::endl;
        return sst.str();
    }
private:
    WorkNode *w_;
    ReceiveNode c_;
};

class WorkManager {
public:
    WorkManager(std::vector<uint32_t> &writer ){
        std::copy(writer.begin(),writer.end(), std::back_inserter(wr_));
    }
    WorkManager(std::vector<uint32_t> &writer,std::vector<std::vector<uint32_t>> &recivers ){
        assert(recivers.size()>= writer.size());
        std::copy(writer.begin(),writer.end(), std::back_inserter(wr_));
        for(size_t i = 0;i<wr_.size();i++){
            vec_.push_back(WorkPair(&wr_[i],wr_[i].Value()));
        }
        for(size_t i=0;i <wr_.size() ;i++){
            for(size_t j=0;j<recivers[i].size();j++){
                vec_.push_back(WorkPair(&wr_[i],recivers[i][j]));
            }
        }
    }
    /**
     * @brief 选择其中两个以上的writer，进行交换信息，选择三个case进行接受信息
     * 
     * @param selectwrite 
     * @param suballcases 
     */
    uint32_t Init(std::vector<std::vector<uint32_t>> & recivers,std::vector<epaxos_test::Case_Node> &csn){ //生产者的数量
        // selectwrite.size() + suballcases.size()
        size_t offset= recivers[0].size();
        vec_.clear();
        for(size_t i = 0;i<wr_.size();i++){
            vec_.push_back(WorkPair(&wr_[i],wr_[i].Value()));
            csn.emplace_back(epaxos_test::Case_Node(i,i*(1+offset)+1));
           // std::cout<<"workmanager init :"<<i<<" " << i*(1+offset)+1<< std::endl;
        }
        size_t mov = csn.size();
        for(size_t i=0;i <wr_.size() ;i++){
            for(size_t j=0;j<recivers[i].size();j++){
                vec_.push_back(WorkPair(&wr_[i],recivers[i][j]));
                csn.emplace_back(epaxos_test::Case_Node( i*offset + j + mov));

             //   std::cout<<"workmanager init :"<<offset<<" " << i*offset + j + mov<< std::endl;
            }
        }
        //将vec进行打乱来得到一个全排列
        return csn.size();
    }
    epaxos::InstanceSwap StepAt(size_t i ){
        assert(i< vec_.size());
        return vec_[i].SwapMsg();
    }
    void Reset(){
        std::for_each(wr_.begin(),wr_.end(),[](WorkNode &a){
            a.Reset();
            return ;
        });
    }
    std::string GetDetail(){
       return PrintfVectorN<WorkPair>(vec_);
    }

private:
    std::vector<WorkPair> vec_;
    std::vector<WorkNode> wr_;
};

};

#endif