

#ifndef __EPAXOS_INSTANCE_STATUS_H__
#define __EPAXOS_INSTANCE_STATUS_H__
#include "../include/comminclude.h"
namespace epaxos {

class WorkStRes{
public:
    WorkStRes():iRet_(EM_INS_TRANF_SUCCESS){}

    bool IsSuccess(){return iRet_ == EM_INS_TRANF_SUCCESS;}

    static WorkStRes BuildSmallerState(){ return WorkStRes(EM_INS_TRANF_SMLLER_STATE);}
private:
    
    enum emInstanceTransferState{
        EM_INS_TRANF_SUCCESS = 0 ,
        EM_INS_TRANF_SMLLER_STATE = 1,
    };
    emInstanceTransferState iRet_;

    WorkStRes(emInstanceTransferState c):iRet_(c){}
};

class WorkStatus{

enum emPaxosState{
    EM_EMPTY = 0,
    EM_STATUS_PROPOSE = 1,
    EM_STATUS_PREACCEPT = 2,
    EM_STATUS_ACCEPT = 3,
    EM_STATUS_COMMIT= 4,
    EM_STATUS_EXEC_DONE= 5,
};
public:
    WorkStatus():st_(EM_EMPTY){}
    
    WorkStRes StepToNext();
    static WorkStRes StepToProPose(WorkStatus &a){
        a.st_ = EM_STATUS_PROPOSE;
        return WorkStRes();
    }
    WorkStRes StepToPreAccept();
    WorkStRes StepToAccept();
    WorkStRes StepToCommit();

    bool IsPreAccept()const { return st_==EM_STATUS_PROPOSE; }
    bool IsAccept() const{ return st_== EM_STATUS_ACCEPT; }
    bool IsCommit()const { return st_== EM_STATUS_COMMIT;}
    bool IsExec()const { return st_ ==EM_STATUS_EXEC_DONE; }

    bool operator < (const WorkStatus &a)const { return st_ < a.st_;}
    bool operator == (const WorkStatus &a)const { return st_ == a.st_;}
    bool Behand(const WorkStatus&a){ return a.st_ <= st_;}
    
private:
    bool Invalid( uint32_t st)const;

private:
    uint32_t st_;    //本地的状态，枚举值
};

};

#endif