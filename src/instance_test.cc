#include <gtest/gtest.h>

#include "instance.pb.h"


TEST(InitInstanceTest, BasicInstance){
  epxos_instance_proto::EpInstance ins;
  ins.set_ack_num(1234);

  EXPECT_EQ(ins.ack_num(),1);
}

int main(int argc ,char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}