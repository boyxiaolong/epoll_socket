#include <gtest/gtest.h>

#include "../include/net_timer.h"
#include "../include/log.h"

class net_timer_unittest : public ::testing::Test {
public:

};

TEST_F(net_timer_unittest, create_timer) {
    LOG("");
    EXPECT_TRUE(net_timer::get_instance() != nullptr);
}