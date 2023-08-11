#include <ctime>
#include <string>
#include <iostream>

#include "include/rescode.h"
#include "include/comminclude.h"
#include "include/g_conf.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "src/instance_mng.h"

#include "src/node_num_cfg.h"

#include <ios>
#include <fstream>

std::string kvdbfile("./testdb");

void print_localtime() {
  std::time_t result = std::time(nullptr);
  std::cout << std::asctime(std::localtime(&result));
}


int main(int argc, char** argv) {
  std::string who = "vapourchen";

  int start=0;
  int offset=0;
  if (argc >= 3) {
    start = atoi(argv[1]);
    offset = atoi(argv[2]);
  }

  epaxos::GEpLobalConf gf;
  gf.init_log();

  epaxos::NodeLocalInstanceId  localinstmng;
  localinstmng.InitLocalCf();
  /*
  epxos_instance_proto::EpGlobalConf ecf;
  ecf.mutable_local_nodeid()->set_id(123);
  epxos_instance_proto::EnNodeInfo *t = ecf.add_node_list();
  t->set_ip("190.0.0.1");
  t->set_port(345);

  localinstmng.UpdatelocalCf(ecf);

  

  std::cout<<localinstmng.GetLocalCf().DebugString()<<std::endl;


  std::cout<<localinstmng.IncId()<<std::endl;
  return 1;

  */

  epxos_instance_proto::EpNodeId nd;
  nd.set_id(3);

  epaxos::LeveldbStorageKv *ldb_ = new epaxos::LeveldbStorageKv("./testdb");

  epaxos::BatchGetKvValueArray* bgv = new epaxos::BatchGetKvValueArray(ldb_);

  epxos_instance_proto::EpInstance ins;

  ins.mutable_iid()->set_insid(localinstmng.IncId());

  ins.mutable_iid()->mutable_nodeid()->CopyFrom(nd);

  for(size_t i=0 ;i<= 10; i++){
    std::string key("vvvsapp" + std::to_string(i));
    epxos_instance_proto::EpKeyValueItem* tmp = ins.mutable_depsids()->add_item();
    tmp->set_key(key);
    tmp->mutable_iid()->CopyFrom(ins.iid());
  }

  bgv->GenNewInsMaxSeqID(ins);

  std::cout<<ins.DebugString()<<std::endl;


/*
  epaxos::NodeNumCfg<uint64_t> nt("test.txt");

  nt.writeins(offset,start);
  uint64_t r= nt.readins(offset);
  std::cout<< "res "<< r <<std::endl;
*/


  /*
  
  spdlog::error("Some error message with arg: {}", 1);

  spdlog::warn("Easy padding in numbers like {:08d}", 12);
  spdlog::critical("Support for int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42);
  spdlog::info("Support for floats {:03.2f}", 1.23456);
  spdlog::info("Positional args are {1} {0}..", "too", "supported");
  spdlog::info("{:<30}", "left aligned222");
  */

  print_localtime();
  return 0;
}
