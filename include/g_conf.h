#ifndef __EPAXOS_G_CONF_H__
#define __EPAXOS_G_CONF_H__
#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"

namespace epaxos{

class GLobalConf{

public:
    void g_init(){
        //日志初始化
        auto logger = spdlog::create<spdlog::sinks::basic_file_sink_mt>("logger", "./log/basic_log.txt");
        logger->set_pattern("[%H:%M:%S.%e] [%^%L%$] [%s:%#] %v");
        logger->set_level(spdlog::level::trace);
        spdlog::set_default_logger(logger);
    }

private:

};

};
#endif 