#include <gtest/gtest.h>
#include "proto/instance.pb.h"

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(HelloTest2, BasicAssertions2) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hel2lo", "hello");
  // Expect equality.
  EXPECT_EQ(7 * 6, 241);
}

TEST(HelloTest3, BasicAssertions3) {
  epxos_instance_proto::EpInstance ins;
  ins.set_ack_num(1234);

  EXPECT_NE(ins.ack_num(),1);
}


