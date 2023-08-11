#ifndef __EPAXOS_G_CONF_H__
#define __EPAXOS_G_CONF_H__
#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"
#include <memory>

namespace epaxos{

class GEpLobalConf{

public:
    void init_log(std::string filename=""){
        if(filename == "") {
            filename =  "./log/basic_log.txt";
        }
        //日志初始化
        auto logger_ = spdlog::create<spdlog::sinks::basic_file_sink_mt>("logger",filename);
        //logger->set_pattern("[%H:%M:%S.%e] [%n] [%^%L%$] [%s:%#] [thread %t] %v");
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S] [%^%L%$] [%s:%#] %v");
        logger_->set_level(spdlog::level::trace);
        spdlog::set_default_logger(logger_);
        spdlog::info("Welcome to spdlog!");
    }
private:
    //std::shared_ptr<spdlog::logger> logger_;

};

};
#endif 