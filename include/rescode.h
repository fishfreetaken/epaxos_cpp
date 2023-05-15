

#ifndef __EPAXOS_INCLUE_RESCODE__
#define __EPAXOS_INCLUE_RESCODE__
#include "comminclude.h"

class ResCode {

public: 
    ResCode(int i):iCode_(i){}
    int GetCode(){return iCode_;}
    std::string  GetRemote(){return strRemote;}

    
private:    
    
    //new instance 
    int iCode_;             //错误码
    std::string strRemote;  //提示
};


#endif