#include <stdio.h>

#include <TrivialTestKit.h>
#include <ToStr.h>

#include <Logger.h>

//------------------------------------------------------------------------------

bool IsFileExists(const std::string& file_name) {
    const DWORD attributes = GetFileAttributesW(ToUTF16(file_name).c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool DeleteFileUTF8(const std::string& file_name) {
    return DeleteFileW(ToUTF16(file_name).c_str());
}

//------------------------------------------------------------------------------

void TestLoggerOpenFile() {
    TTK_ASSERT(CreateDirectoryA(".\\log", 0) || GetLastError() == ERROR_ALREADY_EXISTS);
    TTK_ASSERT(CreateDirectoryA(".\\log\\test", 0) || GetLastError() == ERROR_ALREADY_EXISTS);

    // empty file
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

    // replace file
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

    // replace file, text utf8
    {
        const std::string file_name = "log\\test\\LoggerOpenFile_CreateAndLog_TextUTF8.txt";
        DeleteFileA(file_name.c_str());

        Logger logger;
        logger.OpenFile(file_name, false);
        logger.LogText(u8"Some text\u0444.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT(LoadTextFromFile(file_name) == u8"Some text\u0444.\n");

        logger.OpenFile(file_name, false);
        logger.LogText(u8"Another text\u0444.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT(LoadTextFromFile(file_name) == u8"Another text\u0444.\n");
    }

    // replace file, file name and text utf8
    {
        const std::string file_name = u8"log\\test\\LoggerOpenFile_CreateAndLog_TextUTF8\u0444.txt";
        DeleteFileUTF8(file_name);

        Logger logger;
        logger.OpenFile_FileNameUTF8(file_name, false);
        logger.LogText(u8"Some text\u0444.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT_M(LoadTextFromFileUTF8(file_name) == u8"Some text\u0444.\n", LoadTextFromFileUTF8(file_name));

        logger.OpenFile_FileNameUTF8(file_name, false);
        logger.LogText(u8"Another text\u0444.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT_M(LoadTextFromFileUTF8(file_name) == u8"Another text\u0444.\n", LoadTextFromFileUTF8(file_name));
    }

    // append file
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