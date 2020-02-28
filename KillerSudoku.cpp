#include "NumberSet.h"
#include <string>
#include "gtest/gtest.h"
using namespace std;

TEST(Testing, Works)
{
    EXPECT_EQ(true, true);
}

int test_main(int argc, char** argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

int main(int argc, char** argv)
{
    return test_main(argc, argv);
}
