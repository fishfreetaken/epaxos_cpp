

namespace epaxos {

class Status{
    enum emPaxosState{
        EM_STATUS_PROPOSE = 1,
        EM_STATUS_PREACCEPT = 2,
        EM_STATUS_ACCEPT = 3,
        EM_STATUS_COMMIT= 4,
    };
public:
    void StepToNext();
    void StepToProPose();
    void StepToPreAccept();
    void StepToAccept();
    void StepToCommit();

    bool IsPreAccept();
    
private:
    uint32_t state_;    //本地的状态，枚举值
};

};