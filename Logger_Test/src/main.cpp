#include <stdio.h>

#include <TrivialTestKit.h>
#include <ToStr.h>

#include <Logger.h>

void TestDummy() {
    TTK_ASSERT(ToStr("%s", "abc") == "abc");
}

int main() {
    TTK_ADD_TEST(TestDummy, 0);
    TTK_Run();
    return 0;
}