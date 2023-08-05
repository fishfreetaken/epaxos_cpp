

#ifndef __EPAXOS_INCLUE_RESCODE__
#define __EPAXOS_INCLUE_RESCODE__
#include <string>
#include <spdlog/spdlog.h>
#include <cassert>

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
    const IfChange& operator =(const IfChange &a){
        if(a.val_){
            val_ = true;
        }
        return *this; 
    }
private:
    bool val_;
};

class ResCode:public IfChange {

public: 
    ResCode():iCode_(0){}
    ResCode(int i):iCode_(i){}
    ResCode(int i,std::string s):iCode_(i),strPromote(s){}

    std::string  GetRemote(){return strPromote;}

    static ResCode Success(){return ResCode(0);}

    static ResCode DBReadErr(){return ResCode(-1,"DB Read Failed");}
    static ResCode DBWriteErr(){return ResCode(-2,"DB Write failed");}
    static ResCode NotFound(){return ResCode(-3,"Not found");}
    static ResCode DecodeErr(){return ResCode(-3,"Decode failed");}
    static ResCode EncodeErr(){return ResCode(-3,"Encode failed");}

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
    enum emInstanceCodeState{
        EM_INS_STATE_EXIST = 1, //0 not EXIST
    };

    //new instance 
    int iCode_;             //错误码
    std::string strPromote;  //提示
};


}

#endif