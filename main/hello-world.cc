#include <ctime>
#include <string>
#include <iostream>

#include "include/rescode.h"

#include "src/instance_node.h"

void print_localtime() {
  std::time_t result = std::time(nullptr);
  std::cout << std::asctime(std::localtime(&result));
}

int main(int argc, char** argv) {
  std::string who = "world";
  if (argc > 1) {
    who = argv[1];
  }
  epaxos::ResCode res(1232432);

  std::cout << "rescode "<< res.IsError()<<std::endl;

  epaxos::TestInstanceNode ttl(2342);

  std::cout << "TestInstanceNode " << ttl.GetValue() << " "<<ttl.SetValue(1111)<<std::endl;

  std::cout << "Test tt hello world" << ttl.SetEpInsId(903940) << " "<<ttl.GetEpInsId()<<std::endl;

  print_localtime();
  return 0;
}
