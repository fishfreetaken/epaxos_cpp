

#include "status.h"

namespace epaxos{

    bool WorkStatus::Invalid( uint32_t st)const{ 
        
        return st <= state_ ;
    }
};