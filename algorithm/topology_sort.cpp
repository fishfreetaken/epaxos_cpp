
#include "topology_sort.h"

namespace epaxos {

void topologysort::DoSort(std::vector<Node*> &res){
    std::deque<uint64_t> zerolist;
    std::vector<std::vector<uint64_t>> ans(m_score.size());
    uint32_t count=0;
    while(zerolist.size()>0){
        std::vector<uint64_t> tmp;
        for(;true;){
            auto top = zerolist.front();
            auto iter = m_score.find(top);
            if (iter == m_score.end()){
                continue;
            }
            iter->second--;
            if(iter->second==0){
                zerolist.push_back(iter->first);
                tmp.push_back(iter->first);
                m_score.erase(iter);
            }
        }
        count++;
        assert(tmp.size()>0);
        ans.push_back(tmp);
    }
}


};