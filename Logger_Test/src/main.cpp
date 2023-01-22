#include <stdio.h>

#include <TrivialTestKit.h>
#include <ToStr.h>

#include <Logger.h>

//------------------------------------------------------------------------------

bool IsFileExists(const std::string& file_name) {
    const DWORD attributes = GetFileAttributesA(file_name.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

//------------------------------------------------------------------------------

void TestLoggerOpenFile() {
    TTK_ASSERT(CreateDirectoryA(".\\log", 0) || GetLastError() == ERROR_ALREADY_EXISTS);
    TTK_ASSERT(CreateDirectoryA(".\\log\\test", 0) || GetLastError() == ERROR_ALREADY_EXISTS);

    {

        const std::string file_name = "log\\test\\LoggerOpenFile_CreateEmpty.txt";
        DeleteFileA(file_name.c_str());

        Logger logger;
        TTK_ASSERT(logger.IsFileOpened() == false);
        logger.OpenFile(file_name, false);
        TTK_ASSERT(logger.IsFileOpened() == true);
        logger.CloseFile();
        TTK_ASSERT(logger.IsFileOpened() == false);

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT(LoadTextFromFile(file_name) == "");
    }

    {
        const std::string file_name = "log\\test\\LoggerOpenFile_CreateAndLog.txt";
        DeleteFileA(file_name.c_str());

        Logger logger;
        logger.OpenFile(file_name, false);
        logger.LogText("Some text.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT(LoadTextFromFile(file_name) == "Some text.\n");

        logger.OpenFile(file_name, false);
        logger.LogText("Another text.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT(LoadTextFromFile(file_name) == "Another text.\n");
    }

    {
        const std::string file_name = "log\\test\\LoggerOpenFile_CreateAndLogAppend.txt";
        DeleteFileA(file_name.c_str());

        Logger logger;
        logger.OpenFile(file_name, false);
        logger.LogText("Some text.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT(LoadTextFromFile(file_name) == "Some text.\n");

        logger.OpenFile(file_name, true);
        logger.LogText("Another text.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT(LoadTextFromFile(file_name) == "Some text.\nAnother text.\n");
    }
}

//------------------------------------------------------------------------------


int main() {
    TTK_ADD_TEST(TestLoggerOpenFile, 0);
    TTK_Run();
    return 0;
}