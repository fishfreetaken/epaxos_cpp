#include <ctime>
#include <string>
#include <iostream>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "include/rescode.h"

#include "include/comminclude.h"

#include <leveldb/db.h>

#include "spdlog/sinks/basic_file_sink.h"

//#include "src/leveldb_kv.h"

#include "src/instance_mng.h"

std::string kvdbfile("./testdb");

void print_localtime() {
  std::time_t result = std::time(nullptr);
  std::cout << std::asctime(std::localtime(&result));
}

int main(int argc, char** argv) {
  std::string who = "world";
  if (argc > 1) {
    who = argv[1];
  }

  epaxos::LeveldbStorageKv ldb(kvdbfile);
  std::string key("vapourchen");
  epaxos::BatchGetKvValueArray bgv(&ldb);

  std::vector<std::string> vct;
  for(int i=0;i<10;i++){
    vct.push_back(key+ std::to_string(i));
  }

  epxos_instance_proto::EpInstID insid;

  bgv.BatchAtoicKeyValueSeqId(vct,&insid);

  for(int i=0;i<10;i++){
    epxos_instance_proto::EpValueItem v;
    bgv.Get(vct[i],v);
    spdlog::info("i{} info{}!",i,11);
  }
  
  /*
  std::string value;
  epaxos::ResCode r = ldb.read(key,value);
  spdlog::info("key:{}, value:{}, rsp:{}",key,value,r.IsError());

  value= "iniaji";
  ldb.write(key, value);

  r = ldb.read(key,value);
  spdlog::info("key:{}, value:{}, rsp:{}",key,value,r.IsError());

  std::unordered_map<std::string,std::string> ump;
  ump[key+"1"]="1";
  ump[key+"2"]="2";
  ump[key+"3"]="3";
  ump[key+"4"]="4";
  ldb.batchwrite(ump);

  for(auto iter=ump.begin();iter!=ump.end();iter++){
    r= ldb.read(iter->first,value);
    spdlog::info("key:{}, value:{}, rsp:{}",iter->first,value,r.IsError());
  }
  */


  spdlog::info("Welcome to spdlog!");
  spdlog::error("Some error message with arg: {}", 1);

  spdlog::warn("Easy padding in numbers like {:08d}", 12);
  spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
  spdlog::info("Support for floats {:03.2f}", 1.23456);
  spdlog::info("Positional args are {1} {0}..", "too", "supported");
  spdlog::info("{:<30}", "left aligned222");

  spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");


  auto logger = spdlog::create<spdlog::sinks::basic_file_sink_mt>("logger", "./log/basic_log.txt");
  logger->set_pattern("[%H:%M:%S.%e] [%^%L%$] [%s:%#] %v");
  logger->set_level(spdlog::level::trace);
  spdlog::set_default_logger(logger);
  

  SPDLOG_TRACE("Some trace message with param {} {}", 42,"hello world");
  SPDLOG_DEBUG("Some debug message");

  print_localtime();
  return 0;
}
