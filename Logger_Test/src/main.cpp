#include <stdio.h>

#include <TrivialTestKit.h>
#include <Logger.h>

void TestDummy() {

}

int main() {
    TTK_ADD_TEST(TestDummy, 0);
    TTK_Run();
    return 0;
}