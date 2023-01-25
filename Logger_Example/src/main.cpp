#if 1
#include <Logger.h>

int main() {
    Logger logger;
    logger.OpenFile("log.txt", false);

    logger.LogText("=== Logs ===\n");
    logger.LogDump("Some dump message.");
    logger.LogEvent("Some event message.");
    logger.LogWarning("Some warning message.");
    logger.LogError("Some error message.");

    logger.CloseFile();

    return 0;
}
#endif

#if 0
#include <Logger.h>

int main() {
    Logger logger;
    logger.OpenStdOut();

    logger.LogText("=== Logs ===\n");
    logger.LogDump("Some dump message.");
    logger.LogEvent("Some event message.");
    logger.LogWarning("Some warning message.");
    logger.LogError("Some error message.");

    logger.CloseStdOut();

    return 0;
}
#endif

#if 0
#include <Logger.h>

int main() {
    Logger logger;

    logger.OpenFile("log.txt", false);
    logger.OpenStdOut();

    logger.LogText("=== Logs ===\n");
    logger.LogDump("Some dump message.");
    logger.LogEvent("Some event message.");
    logger.LogWarning("Some warning message.");
    logger.LogError("Some error message.");

    logger.CloseStdOut();
    logger.CloseFile();

    return 0;
}
#endif

#if 0
#include <Logger.h>

int main() {
    Logger logger;

    logger.OpenFile("log.txt", false);

    logger.LogText("=== Logs ===\n");
    logger.LogDump("Some dump message.");
    logger.LogEvent("Some event message.");

    logger.LogFatalError("Some fatal error message."); // Exits with error code -1 after logging.

    // This messages don't get through.
    logger.LogWarning("Some warning message.");
    logger.LogError("Some error message.");

    logger.CloseFile();

    return 0;
}
#endif

#if 0
#include <Logger.h>

int main() {
    Logger logger;

    logger.SetDoAtFatalError([](const char* message) {
        MessageBoxA(NULL, message, "Fatal Error", MB_ICONERROR);
    });

    logger.OpenFile("log.txt", false);

    logger.LogText("=== Logs ===\n");
    logger.LogDump("Some dump message.");
    logger.LogEvent("Some event message.");

    logger.LogFatalError("Some fatal error message."); // Exits with error code -1 after logging.

    // This messages don't get through.
    logger.LogWarning("Some warning message.");
    logger.LogError("Some error message.");

    logger.CloseFile();

    return 0;
}
#endif

#if 0
#include <Logger.h>

int main() {
    Logger logger;

    logger.OpenFile("log.txt", false);

    logger.LogText("=== %s ===\n", "Logs");
    logger.LogDump("Some %s message %d.", "dump", 1);
    logger.LogEvent("Some %s message %d.", "event", 2);
    logger.LogWarning("Some %s message %d.", "warning", 3);
    logger.LogError("Some %s message %d.", "error", 4);

    logger.CloseFile();

    return 0;
}
#endif


#if 0
#include <Logger.h>

int main() {
    Logger logger;

    logger.OpenFile("log.txt", false);

    logger.LogText("=== %s ===\n", "Logs");
    logger.LogDump("Some %s message %d.", "dump", 1);
    logger.LogEvent("Some %s message %d.", "event", 2);

    logger.LogFatalError("Some %s message %d.", "fatal error", 100); // Exits with error code -1 after logging.

    logger.LogWarning("Some %s message %d.", "warning", 3);
    logger.LogError("Some %s message %d.", "error", 4);

    logger.CloseFile();

    return 0;
}
#endif

#if 0
#include <Logger.h>

int main() {
    Logger logger;

    logger.OpenFile("log.txt", false);
    logger.Enable(LoggerOption::LOG_TIME);

    logger.LogText("=== Logs ===\n");
    logger.LogDump("Some dump message.");
    logger.LogEvent("Some event message.");
    logger.LogWarning("Some warning message.");
    logger.LogError("Some error message.");

    logger.CloseFile();

    return 0;
}
#endif

// LOGGER_TRACK - logs entering into a function and exiting from a function, needs to be placed at beginning of function
// LOGGER_TRACE - logs message with additional information in which function was called

#if 0
#include <Logger.h>

Logger g_logger;

class SomeClass {
public:
    SomeClass() {
        LOGGER_TRACK(g_logger);
    }
    virtual ~SomeClass() {
        LOGGER_TRACK(g_logger);

    }
    void Method() {
        LOGGER_TRACK(g_logger);

        LOGGER_TRACE(g_logger, "Some trace message.");
    }
private:
};

int main() {
    g_logger.OpenFile("log.txt", false);

    {
        SomeClass some_object;
        some_object.Method();
    }

    g_logger.CloseFile();

    return 0;
}
#endif

#if 0
#include <Logger.h>

int main() {
    Logger logger;

    logger.OpenFile("log.txt", false);

    // logger.Disable(LoggerOption::LOG_TRACE);
    logger.Disable(LoggerOption::LOG_DUMP);
    // logger.Disable(LoggerOption::LOG_EVENT);
    logger.Disable(LoggerOption::LOG_WARNING);

    logger.LogText("=== Logs ===\n");
    logger.LogDump("Some dump message.");
    logger.LogEvent("Some event message.");
    logger.LogWarning("Some warning message.");
    logger.LogError("Some error message.");

    logger.CloseFile();

    return 0;
}
#endif