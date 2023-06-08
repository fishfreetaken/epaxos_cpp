

#ifndef __EPAXOS_INCLUE_RESCODE__
#define __EPAXOS_INCLUE_RESCODE__
#include "comminclude.h"
namespace epaxos {

class IfChange{
public:
    IfChange():val_(false){}
    IfChange(bool b):val_(b){}
    void Refresh(const IfChange &c){
        val_ = c.val_ ? c.val_:val_;
    }
    bool IsChange()const {return val_;}
    void Change(){ val_ = true;}

    bool Value()const{ return val_;}
private:
    bool val_;
};

class ResCode:public IfChange {

public: 
    ResCode():iCode_(0){}
    ResCode(int i):iCode_(i){}
    ResCode(std::string s):iCode_(0),strPromote(s){}

    std::string  GetRemote(){return strPromote;}

    void RefreshCode(ResCode c){
        Refresh(c);
        //上报优先级更高的错误
        iCode_ = c.iCode_ >  iCode_ ? c.iCode_ : iCode_;
    }
    bool IsError(){
        if((iCode_ !=0 )||(strPromote.size()>0)){
            return true;
        }
        return false;
    }
private:
    
    //new instance 
    int iCode_;             //错误码
    std::string strPromote;  //提示
};


}

#endif