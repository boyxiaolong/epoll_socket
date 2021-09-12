#include <gtest/gtest.h>

#include "../include/net_timer.h"

class net_timer_unittest : public ::testing::Test {
};

TEST_F(net_timer_unittest, create_timer) {
    EXPECT_TRUE(net_timer::get_instance() != nullptr);
}