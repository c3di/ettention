#include "stdafx.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
#ifdef _WINDOWS
    system("pause");
#endif
    return result;
}
