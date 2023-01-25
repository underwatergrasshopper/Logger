# Description
A C++ library which provides functionality to log a messages to a file, like: tracks, dumps, events, warning, errors, etc.

Targeted platform: Windows.

## HOWTO: Proper clone repository with Git
Run:
```
git clone --recurse-submodules https://github.com/underwatergrasshopper/Logger.git
```
.

## HOWTO: Run tests manually
To run all tests for Visual Studio\*, call `Test.bat`.             
To run all tests for MinGW\*\*, call `MinGW_Test.bat`.     
To run selected tests for MinGW\*\*, call `MinGW_Test.bat run <build_target> <architecture>` from `Logger_Test` folder, where
```
<build_target>
    Release
    Debug
<architecture>
    64
    32
```
.
<sup>\*) To be able compile, add path to MSBuild bin folder in `Logger/TestCache.bat`:
```
set MSBUILD_PATH=<path>
```
.
</sup>        
<sup>\*\*) To be able compile with 64 bit and 32 bit gcc, add paths to mingw bin folder in `Logger/Logger_Test/MinGW_MakeCache.bat`:
```
set MINGW32_BIN_PATH=<path>
set MINGW64_BIN_PATH=<path>
```
.
</sup>

## Builds and tests results

Compiler: **MSVC** (automated)

| Test Platform | Target Platform | Environment | Architecture | Build and Test |
|-|-|-|-|-|
| Windows Server 2022, Windows Server 2019 | Windows 10 |  Visual Studio 2022, Visual Studio  2019 | x86, x86_64 | [![Build and Test](https://github.com/underwatergrasshopper/Logger/actions/workflows/build_and_test.yml/badge.svg)](https://github.com/underwatergrasshopper/Logger/actions/workflows/build_and_test.yml) |

 Compiler: **LLVM CLang MinGW** (automated)

| Test Platform | Target Platform | Environment | Architecture | Build and Test |
|-|-|-|-|-|
| Windows Server 2022 | Windows 10 | MinGW-W64 (LLVM Clang 15.0.0) | x86, x86_64 |  [![Build and Test (MinGW)](https://github.com/underwatergrasshopper/Logger/actions/workflows/build_and_test_mingw.yml/badge.svg)](https://github.com/underwatergrasshopper/Logger/actions/workflows/build_and_test_mingw.yml) |

# Examples 

## Logging to file
Logs messages to `log.txt`. Calling `CloseFile` is optional, file is closed at `logger` object destruction.
Function `LogText` logs raw not formated text.

```c++
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
```
Content of *log.txt*:
```
=== Logs ===
[Dump]: Some dump message.
[Event]: Some event message.
[Warning]: Some warning message.
[Error]: Some error message.
```


## Logging to standard output
Logs messages to standard output (stdout).

```c++
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
```

... and logging to stdout and to file at the same time ...

```c++
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
```

## Logging fatal error
Logs fatal error and exits executable with error code -1 right after fatal error is logged.

```c++
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
```
Content of *log.txt*:
```
=== Logs ===
[Dump]: Some dump message.
[Event]: Some event message.
[Fatal Error]: Some fatal error message.
```

Function can be added which will be executed at call `LogFatalError`, just before exiting from executable.

```c++
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
```

## Logging messages witch arguments
Additional arguments can be added to message which they will be converted to strings.
Format for conversion is the same as for `printf` function.

```c++
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
```
Content of *log.txt*:
```
=== Logs ===
[Dump]: Some dump message 1.
[Event]: Some event message 2.
[Warning]: Some warning message 3.
[Error]: Some error message 4.
```


```c++
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
```
Content of *log.txt*:
```
=== Logs ===
[Dump]: Some dump message 1.
[Event]: Some event message 2.
[Fatal Error]: Some fatal error message 100.
```

## Logging with timestamps
Logging with timestamps can be enabled by calling `Enable(LoggerOption::LOG_TIME)`.

```c++
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
```
Content of *log.txt*:
```
=== Logs ===
[2023/01/25 11:54:35][Dump]: Some dump message.
[2023/01/25 11:54:35][Event]: Some event message.
[2023/01/25 11:54:35][Warning]: Some warning message.
[2023/01/25 11:54:35][Error]: Some error message.
```

## Tracking execution of code
There are special macros to track execution in code:
- `LOGGER_TRACK` - logs entering into a function and exiting from a function, needs to be placed at beginning of function,
- `LOGGER_TRACE` - logs message with additional information in which function was called.

This type of logging can be disabled by calling `Disable(LoggerOption::LOG_TRACE)`.

```c++
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
```
Content of *log.txt*:
```
[Trace][SomeClass::SomeClass]: Enter.
[Trace][SomeClass::SomeClass]: Exit.
[Trace][SomeClass::Method]: Enter.
[Trace][SomeClass::Method]: Some trace message.
[Trace][SomeClass::Method]: Exit.
[Trace][SomeClass::~SomeClass]: Enter.
[Trace][SomeClass::~SomeClass]: Exit.
```


## Logging without specific log message types
Specific log message types can be disabled from logging. Those messages types are: Trace/Track, Dump, Event, Warning.

```c++
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
```
Content of *log.txt*:
```
=== Logs ===
[Event]: Some event message.
[Error]: Some error message.
```