#include <gtest/gtest.h>
#include "proto/instance.pb.h"
#include "src/instance_mng.h"
#include "include/g_conf.h"

size_t rangeEnd=10;
//::testing::tuple<bool, int> 
class InstanceMngBatchKvArrTest :public testing::TestWithParam<::testing::tuple<int,int>> {
protected:
  static void SetUpTestSuite() {
    ldb_ = new epaxos::LeveldbStorageKv("./testdb");
    gf_ = new epaxos::GEpLobalConf();
    ASSERT_NE(gf_,nullptr);
    gf_->init_log();
    ASSERT_NE(ldb_,nullptr);

    raw_key = "vvapp";
  }

  static void TearDownTestSuite() {
    delete ldb_;
    ldb_=nullptr;
    delete gf_;
    gf_=nullptr;
  }

  // You can define per-test set-up logic as usual.
  void SetUp() override { 
    bgv = new epaxos::BatchGetKvValueArray(ldb_);
    //获取内部的值的最大值
    ASSERT_NE(bgv,nullptr);
    if(st_g_seq==-1){
      st_g_seq = GetSeqId(0,10);
    }
  }

  // You can define per-test tear-down logic as usual.
  void TearDown() override { 
    delete bgv;
    bgv = nullptr;
  }

  int64_t GetSeqId(size_t start,size_t end) {
    epxos_instance_proto::EpInstance insid;
    bgv->GenNewInsMaxSeqID(insid);
    return insid.iid().seqid();
  }

  void GetInsInfo(size_t start,size_t end,epxos_instance_proto::EpInstance& ins){
    for(size_t i=start ;i<= end; i++){
      std::string key(raw_key + std::to_string(i));
      epxos_instance_proto::EpKeyValueItem* tmp = ins.mutable_depsids()->add_item();
      tmp->set_key(key);
    }
  }

  static epaxos::LeveldbStorageKv *ldb_;

  static epaxos::GEpLobalConf *gf_;

  static std::string raw_key;

  static int64_t st_g_seq;

  epaxos::BatchGetKvValueArray *bgv;
};

epaxos::LeveldbStorageKv * InstanceMngBatchKvArrTest::ldb_ = nullptr;
epaxos::GEpLobalConf* InstanceMngBatchKvArrTest::gf_=nullptr;
std::string InstanceMngBatchKvArrTest::raw_key;
int64_t InstanceMngBatchKvArrTest::st_g_seq = -1;

// Demonstrate some basic assertions.
TEST_P(InstanceMngBatchKvArrTest, DonsFirst) {
  // std::cout << "Example Test Param: " << GetParam() << std::endl;
  // Expect two strings not to be equal.

  size_t start = ::testing::get<0>(GetParam());
  size_t end = ::testing::get<1>(GetParam());
  if (start > end){
    return ;
  }

  std::vector<std::string> vct;
  for(size_t i=start ;i<= end; i++){
    vct.push_back(InstanceMngBatchKvArrTest::raw_key + std::to_string(i));
  }

  uint64_t tmp_seq =bgv->GetSeqId(start,end);
  ++st_g_seq;
  EXPECT_EQ(st_g_seq,tmp_seq);

  std::for_each(vct.begin(),vct.end(),[&](const std::string & key){
    epxos_instance_proto::EpValueItem v;
    bgv->Get(key,v);
    EXPECT_EQ(tmp_seq,v.iid().seqid());

    v.mutable_iid()->set_seqid(0);
    bgv->ReadOneFromDb(key,v);
    EXPECT_EQ(tmp_seq,v.iid().seqid());
  });
}

//testing::Range(0, 10)
INSTANTIATE_TEST_SUITE_P(MyGroup, InstanceMngBatchKvArrTest,::testing::Combine(::testing::Range(0,5), ::testing::Range(0, 5)) ,
                         [](const testing::TestParamInfo<InstanceMngBatchKvArrTest::ParamType>& info) {
      return std::to_string(std::get<0>(info.param)) +"_" + std::to_string( std::get<1>(info.param));;
});


