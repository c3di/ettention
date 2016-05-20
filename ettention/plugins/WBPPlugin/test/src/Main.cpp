
#include "gtest/gtest.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    auto result = RUN_ALL_TESTS();
#ifdef _WINDOWS
    system("pause");
#endif
    return result;
}
