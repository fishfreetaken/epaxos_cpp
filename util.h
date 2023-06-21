#ifndef  __EPAXOS_TEST_UTIL__
#define __EPAXOS_TEST_UTIL__
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
template<typename T>
std::string PrintfVectorN(const std::vector<T> & t){
  std::stringstream sst;
  std::for_each(t.begin(),t.end(),[&sst](const T & one){
    sst << one.Value()<<" ";
  });
  return sst.str();
}

std::string PrintfVector(const std::vector<uint32_t> & t){
  std::stringstream sst;

  for(size_t i=0;i<t.size();i++){
    sst<<t[i] <<" ";
  }
  return sst.str();
}

std::string PrintfVector(std::vector<std::vector<uint32_t>> & t){
  std::stringstream sst; 
  std::for_each(t.begin(),t.end(),[&sst](const std::vector<uint32_t> & mt){
      sst <<" "<< PrintfVector(mt)<<std::endl;
  }); 
  return sst.str();
}
#endif