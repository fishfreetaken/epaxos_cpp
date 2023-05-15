
#include "../include/comminclude.h"

namespace epaxos {

class SeqId{
    public:
        SeqId():nodeId_(0){}
        SeqId(uint32_t nodeid):nodeId_(nodeid){};

        bool operator <= (const SeqId& a)const;
        bool operator < (const SeqId& a)const;
        uint32_t GetNodeId()const{return nodeId_;}
        uint64_t GetSeqId() const { return seq_;}
        void UpdateSeq(const SeqId&a);
        std::string GetSeqDetail(){
            std::strstream st;
            st <<  " [node: "<<nodeId_ << " seq: "<<seq_ <<"]";
            return st.str();
        }

    private:
        uint64_t seq_;          //
        uint32_t nodeId_;       //提出的节点id
};


bool SeqId::operator <= (const SeqId& a)const {
    if(a.seq_ == seq_){
        return nodeId_ < a.nodeId_;
    }
    return  seq_ <= a.seq_;
}

bool SeqId::operator < (const SeqId& a)const {
    if(a.seq_ == seq_){
        return nodeId_ < a.nodeId_;
    }
    return  seq_ < a.seq_;
}

void SeqId::UpdateSeq(const SeqId &a){
    seq_ = a.seq_ > seq_ ? a.seq_:seq_;
}

};

