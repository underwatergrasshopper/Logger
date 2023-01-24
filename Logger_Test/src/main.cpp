#include <stdio.h>
#include <set>

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

void TestLoggerOpenCloseFile() {
    TTK_ASSERT(CreateDirectoryA(".\\log", 0) || GetLastError() == ERROR_ALREADY_EXISTS);
    TTK_ASSERT(CreateDirectoryA(".\\log\\test", 0) || GetLastError() == ERROR_ALREADY_EXISTS);

    // empty file
    {
        const std::string file_name = "log\\test\\LoggerOpenCloseFile_CreateEmpty.txt";
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
        const std::string file_name = "log\\test\\LoggerOpenCloseFile_CreateAndLog.txt";
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
        const std::string file_name = "log\\test\\LoggerOpenCloseFile_CreateAndLog_TextUTF8.txt";
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
        const std::string file_name = u8"log\\test\\LoggerOpenCloseFile_CreateAndLog_TextUTF8\u0444.txt";
        DeleteFileUTF8(file_name);

        Logger logger;
        logger.OpenFile(file_name, false);
        logger.LogText(u8"Some text\u0444.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT_M(LoadTextFromFileUTF8(file_name) == u8"Some text\u0444.\n", LoadTextFromFileUTF8(file_name));

        logger.OpenFile(file_name, false);
        logger.LogText(u8"Another text\u0444.\n");
        logger.CloseFile();

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT_M(LoadTextFromFileUTF8(file_name) == u8"Another text\u0444.\n", LoadTextFromFileUTF8(file_name));
    }

    // append file
    {
        const std::string file_name = "log\\test\\LoggerOpenCloseFile_CreateAndLogAppend.txt";
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

void TestLoggerOpenCloseStdOut() {
    // bare open/close
    {
        Logger logger;
        TTK_ASSERT(!logger.IsStdOutOpened());
        logger.OpenStdOut();
        TTK_ASSERT(logger.IsStdOutOpened());
        logger.CloseStdOut();
        TTK_ASSERT(!logger.IsStdOutOpened());
    }

    // log text
    {
        system(".\\Logger_Test.exe LOG_STDOUT > .\\log\\test\\TestLoggerOpenCloseStdOut.txt");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\TestLoggerOpenCloseStdOut.txt") == "Another text.");
    }
}

void TestLoggerLog() {
    {
        const std::string file_name = "log\\test\\TestLoggerLog.txt";
        DeleteFileA(file_name.c_str());

        Logger logger;
        logger.OpenFile(file_name, false);

        logger.LogText("Text.\n");
        LOGGER_TRACE(logger, "Trace.");
        logger.LogDump("Dump.");
        logger.LogEvent("Event.");
        logger.LogWarning("Warning.");
        logger.LogError("Error.");

        logger.LogText("%s %d.\n", "Text", 1);
        LOGGER_TRACE(logger, "%s %d.", "Trace", 2);
        logger.LogDump("%s %d.", "Dump", 3);
        logger.LogEvent("%s %d.", "Event", 4);
        logger.LogWarning("%s %d.", "Warning", 5);
        logger.LogError("%s %d.", "Error", 6);

        logger.CloseFile();

        const std::string expected_text =
            "Text.\n"
            "[Trace][TestLoggerLog]: Trace.\n"
            "[Dump]: Dump.\n"
            "[Event]: Event.\n"
            "[Warning]: Warning.\n"
            "[Error]: Error.\n"
            "Text 1.\n"
            "[Trace][TestLoggerLog]: Trace 2.\n"
            "[Dump]: Dump 3.\n"
            "[Event]: Event 4.\n"
            "[Warning]: Warning 5.\n"
            "[Error]: Error 6.\n";

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT_M(LoadTextFromFile(file_name) == expected_text, LoadTextFromFile(file_name) + "\n" + expected_text);
    }

    // tracker
    {
        const std::string file_name = "log\\test\\TestLoggerLog_Tracker.txt";
        DeleteFileA(file_name.c_str());

        Logger logger;
        logger.OpenFile(file_name, false);

        {
            LOGGER_TRACK(logger);
        }

        logger.CloseFile();

        const std::string expected_text =
            "[Trace][TestLoggerLog]: Enter.\n"
            "[Trace][TestLoggerLog]: Exit.\n";

        TTK_ASSERT(IsFileExists(file_name));
        TTK_ASSERT_M(LoadTextFromFile(file_name) == expected_text, LoadTextFromFile(file_name) + "\n" + expected_text);
    }

    // fatal error
    {
        system(".\\Logger_Test.exe FATAL_ERROR > .\\log\\test\\TestLoggerLog_FATAL_ERROR.txt");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\FATAL_ERROR.txt") == "[Fatal Error]: Some text 5.\n");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\TestLoggerLog_FATAL_ERROR.txt") == "");

        system(".\\Logger_Test.exe FATAL_ERROR_STD_OUT > .\\log\\test\\TestLoggerLog_FATAL_ERROR_STD_OUT.txt");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\FATAL_ERROR_STD_OUT.txt") == "");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\TestLoggerLog_FATAL_ERROR_STD_OUT.txt") == "[Fatal Error]: Some text 5.\n");

        system(".\\Logger_Test.exe FATAL_ERROR_WITH_FUNC > .\\log\\test\\TestLoggerLog_FATAL_ERROR_WITH_FUNC.txt");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\FATAL_ERROR_WITH_FUNC.txt") == "[Fatal Error]: Some text 5.\n");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\TestLoggerLog_FATAL_ERROR_WITH_FUNC.txt") == "Some text 5.");

        system(".\\Logger_Test.exe FATAL_ERROR_ONE_ARG > .\\log\\test\\TestLoggerLog_FATAL_ERROR_ONE_ARG.txt");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\FATAL_ERROR_ONE_ARG.txt") == "[Fatal Error]: Some text.\n");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\TestLoggerLog_FATAL_ERROR_ONE_ARG.txt") == "");

        system(".\\Logger_Test.exe FATAL_ERROR_STD_OUT_ONE_ARG > .\\log\\test\\TestLoggerLog_FATAL_ERROR_STD_OUT_ONE_ARG.txt");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\FATAL_ERROR_STD_OUT_ONE_ARG.txt") == "");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\TestLoggerLog_FATAL_ERROR_STD_OUT_ONE_ARG.txt") == "[Fatal Error]: Some text.\n");

        system(".\\Logger_Test.exe FATAL_ERROR_WITH_FUNC_ONE_ARG > .\\log\\test\\TestLoggerLog_FATAL_ERROR_WITH_FUNC_ONE_ARG.txt");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\FATAL_ERROR_WITH_FUNC_ONE_ARG.txt") == "[Fatal Error]: Some text.\n");
        TTK_ASSERT(LoadTextFromFile(".\\log\\test\\TestLoggerLog_FATAL_ERROR_WITH_FUNC_ONE_ARG.txt") == "Some text.");
    }

    // time
    {
        const std::string file_name = "log\\test\\TestLoggerLog_Time.txt";
        DeleteFileA(file_name.c_str());

        Logger logger;
        logger.Enable(LOGGER_OPTION_LOG_TIME);
        logger.OpenFile(file_name, false);

        logger.LogText("Text.\n");
        LOGGER_TRACE(logger, "Trace.");
        logger.LogDump("Dump.");
        logger.LogEvent("Event.");
        logger.LogWarning("Warning.");
        logger.LogError("Error.");

        logger.CloseFile();

        // No assert. Just to check by eye in file.
    }
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[]) {
    std::set<std::string> flags;

    for (size_t index = 0; index < argc; ++index) {
        flags.insert(argv[index]);
    }

    auto IsFlag = [&flags](const std::string& flag) { 
        return flags.find(flag) != flags.end(); 
    };


    if (IsFlag("FATAL_ERROR")) {
        Logger logger;
        logger.OpenFile(".\\log\\test\\FATAL_ERROR.txt", false);
        logger.LogFatalError("%s %d.", "Some text", 5);
        return 0;

    } else if (IsFlag("FATAL_ERROR_STD_OUT")) {
        Logger logger;
        logger.OpenStdOut();
        logger.LogFatalError("%s %d.", "Some text", 5);
        return 0;

    } else if (IsFlag("FATAL_ERROR_WITH_FUNC")) {
        Logger logger;
        logger.OpenFile(".\\log\\test\\FATAL_ERROR_WITH_FUNC.txt", false);
        logger.SetDoAtFatalError([](const char* message) {
            printf(message);
            fflush(stdout);
        });
        logger.LogFatalError("%s %d.", "Some text", 5);
        return 0;

    } else if (IsFlag("FATAL_ERROR_ONE_ARG")) {
        Logger logger;
        logger.OpenFile(".\\log\\test\\FATAL_ERROR_ONE_ARG.txt", false);
        logger.LogFatalError("Some text.");
        return 0;

    } else if (IsFlag("FATAL_ERROR_STD_OUT_ONE_ARG")) {
        Logger logger;
        logger.OpenStdOut();
        logger.LogFatalError("Some text.");
        return 0;

    } else if (IsFlag("FATAL_ERROR_WITH_FUNC_ONE_ARG")) {
        Logger logger;
        logger.OpenFile(".\\log\\test\\FATAL_ERROR_WITH_FUNC_ONE_ARG.txt", false);
        logger.SetDoAtFatalError([](const char* message) {
            printf(message);
            fflush(stdout);
        });
        logger.LogFatalError("Some text.");
        return 0;

    } else if (IsFlag("LOG_STDOUT")) {
        Logger logger;
        logger.LogText("Some text.");
        logger.OpenStdOut();
        logger.LogText("Another text.");
        logger.CloseStdOut();
        logger.LogText("And another text.");
        return 0;
    } else {
        TTK_ADD_TEST(TestLoggerOpenCloseFile, 0);
        TTK_ADD_TEST(TestLoggerOpenCloseStdOut, 0);
        TTK_ADD_TEST(TestLoggerLog, 0);
        return TTK_Run();
    }
}