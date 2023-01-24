////////////////////////////////////////////////////////////////////////////////
// MIT License
// 
// Copyright (c) 2023 underwatergrasshopper
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

/**
* @file Logger.h
* @author underwatergrasshopper
* @version 2.0.0a1
*/

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <utility>

//------------------------------------------------------------------------------
// Declaration
//------------------------------------------------------------------------------

enum LoggerOption {
    LOGGER_OPTION_LOG_TIME
};

class Logger {
public:
    typedef void (*DoAtFatalErrorFnP_T)(const char* message);

    Logger();
    virtual ~Logger();

    // Opens/Closes logging to file.
    void OpenFile(const std::string& file_name, bool is_append);
    void CloseFile();
    bool IsFileOpened() const;

    // Opens/Closes logging to standard output.
    void OpenStdOut();
    void CloseStdOut();
    bool IsStdOutOpened() const;

    void SetDoAtFatalError(DoAtFatalErrorFnP_T do_at_fatal_error);

    // Enables/Disables:
    //      LOGGER_OPTION_LOG_TIME - logging with timestamps.
    void Enable(LoggerOption option);
    void Disable(LoggerOption option);

    // for all Log{...}() methods:
    // format           Format of log message, same rules as in standard 'printf' function.
    // arguments        Arguments interpreted by format, same rules as in standard 'printf' function.
    // function_name    Name of the function or method where Log{...}() is called.

    void LogText(const std::string& text);

    template <typename... Types>
    void LogText(const std::string& format, Types&&... arguments);

    // Traces execution of function.
    template <typename... Types>
    void LogTrace(const std::string& function_name, const std::string& format, Types&&... arguments);

    template <typename... Types>
    void LogDump(const std::string& format, Types&&... arguments);

    template <typename... Types>
    void LogEvent(const std::string& format, Types&&... arguments);

    template <typename... Types>
    void LogWarning(const std::string& format, Types&&... arguments);

    template <typename... Types>
    void LogError(const std::string& format, Types&&... arguments);

    template <typename... Types>
    void LogFatalError(const std::string& format, Types&&... arguments);

    // Tracks entering and exiting from scope: '{', '}'. 
    // To do so, macro LOGGER_TRACK must be called on beginning of the scope.
    class Tracker {
    public:
        Tracker(Logger& logger, const char* function_name);
        virtual ~Tracker();
    private:
        Logger& m_logger;
        const char* m_function_name;
    };
private:
    template <typename... Types>
    void LogEntry(const std::string& category_name, const std::string& format, Types&&... arguments);

    void LogTime();

    void InnerFatalError(const char* message);

    template <typename... Types>
    std::string GenerateMessage(const std::string& format, Types&&... arguments);

    std::string ToUTF8(const std::wstring& text_utf16);

    FILE*                   m_file;
    bool                    m_is_stdout;
    DoAtFatalErrorFnP_T     m_do_at_fatal_error;
    bool                    m_is_log_time;
};

#define LOGGER_TRACE(logger, ...) logger.LogTrace(__PRETTY_FUNCTION__ , __VA_ARGS__)
#define LOGGER_TRACK(logger) Logger::Tracker l_tracker(logger, __PRETTY_FUNCTION__)

//------------------------------------------------------------------------------
// Definition
//------------------------------------------------------------------------------

inline Logger::Logger() {
    m_file              = nullptr;
    m_is_stdout         = false;
    m_do_at_fatal_error = nullptr;
    m_is_log_time       = false;
}

inline Logger::~Logger() {
    CloseFile();
}

//------------------------------------------------------------------------------

inline void Logger::OpenFile(const std::string& file_name, bool is_append) {
    CloseFile();
    if (fopen_s(&m_file, file_name.c_str(), is_append ? "at" : "wt") != 0 || !m_file) {
        InnerFatalError("Logger: Can not open log file.");
    }
}

inline void Logger::CloseFile() {
    if (m_file) {
        fclose(m_file);
        m_file = nullptr;
    }
}

inline bool Logger::IsFileOpened() const { 
    return m_file != nullptr; 
}

inline void Logger::OpenStdOut() {
    m_is_stdout = true;
}

inline void Logger::CloseStdOut() {
    m_is_stdout = false;
}

inline bool Logger::IsStdOutOpened() const { 
    return m_is_stdout; 
}

//------------------------------------------------------------------------------

inline void Logger::SetDoAtFatalError(DoAtFatalErrorFnP_T do_at_fatal_error) { 
    m_do_at_fatal_error = do_at_fatal_error; 
}

//------------------------------------------------------------------------------

inline void Logger::Enable(LoggerOption option) {
    switch (option) {
    case LOGGER_OPTION_LOG_TIME: m_is_log_time = true; break;
    }
}
inline void Logger::Disable(LoggerOption option) {
    switch (option) {
    case LOGGER_OPTION_LOG_TIME: m_is_log_time = false; break;
    }
}

//------------------------------------------------------------------------------

inline void Logger::LogText(const std::string& text) {
    
    if (m_file) {
        fprintf(m_file, "%s", text.c_str());
        if (errno != 0) {
            InnerFatalError("Logger: Can not open log file.");
        }
        fflush(m_file);
    }
    if (m_is_stdout) {
        if (fwide(stdout, 0) > 0) {
            wprintf(L"%hs", text.c_str());
        } else {
            printf("%s", text.c_str());
        }
        fflush(stdout);
    }
}

template <typename... Types>
void Logger::LogText(const std::string& format, Types&&... arguments) {
    LogText(GenerateMessage(format, std::forward<Types>(arguments)...));
}

template <typename... Types>
void Logger::LogTrace(const std::string& function_name, const std::string& format, Types&&... arguments) {
    if (m_is_log_time) LogTime();
    LogText("[Trace][%s]: ", function_name.c_str());
    LogText(format, std::forward<Types>(arguments)...);
    LogText("\n");
}
template <typename... Types>
void Logger::LogDump(const std::string& format, Types&&... arguments) {
    LogEntry("Dump", format, std::forward<Types>(arguments)...);
} 

template <typename... Types>
void Logger::LogEvent(const std::string& format, Types&&... arguments) {
    LogEntry("Event", format, std::forward<Types>(arguments)...);
} 

template <typename... Types>
void Logger::LogWarning(const std::string& format, Types&&... arguments) {
    LogEntry("Warning", format, std::forward<Types>(arguments)...);
} 

template <typename... Types>
void Logger::LogError(const std::string& format, Types&&... arguments) {
    LogEntry("Error", format, std::forward<Types>(arguments)...);
} 

template <typename... Types>
void Logger::LogFatalError(const std::string& format, Types&&... arguments) {
    if (m_is_log_time) LogTime();
    LogText("[Fatal Error]: ");
    const std::string message = GenerateMessage(format, std::forward<Types>(arguments)...);
    LogText(message);
    LogText("\n");

    if (m_do_at_fatal_error) m_do_at_fatal_error(message.c_str()); 
}

template <typename... Types>
void Logger::LogEntry(const std::string& category_name, const std::string& format, Types&&... arguments) {
    if (m_is_log_time) LogTime();
    LogText("[%s]: ", category_name.c_str());
    LogText(format, std::forward<Types>(arguments)...);
    LogText("\n");
} 

//------------------------------------------------------------------------------

inline void Logger::LogTime() {
    const time_t now = time(NULL);
    tm ti = {};
    localtime_s(&ti, &now);

    LogText("[%d/%02d/%02d %02d:%02d:%02d]", 1900 + ti.tm_year, 1 + ti.tm_mon, ti.tm_mday, ti.tm_hour, ti.tm_min, ti.tm_sec);
}

inline void Logger::InnerFatalError(const char* message) {
    LogText(message);
    if (m_do_at_fatal_error) m_do_at_fatal_error(message);
    exit(EXIT_FAILURE);
}

template <typename... Types>
std::string Logger::GenerateMessage(const std::string& format, Types&&... arguments) {
    std::string message;

    enum { SIZE = 4096 };
    char stack_buffer[SIZE];

    const int count = snprintf(stack_buffer, SIZE, format.c_str(), std::forward<Types>(arguments)...);
    if (count < 0) {
        InnerFatalError("[Inner Fatal Error][Logger::LogText]: Wrong encoding.");
    }

    if (count >= SIZE) {
        char* buffer = new char[count];

        const int second_count = snprintf(buffer, count, format.c_str(), std::forward<Types>(arguments)...);
        if (count < 0) {
            InnerFatalError("[Inner Fatal Error][Logger::LogText]: Wrong encoding (at second try).");
        }
        if (second_count > count) {
            InnerFatalError("[Inner Fatal Error][Logger::LogText]: Can not write to buffer.");
        }

        if (second_count > 0) message = std::string(buffer, second_count);

        delete[] buffer;
    } else {
        message = std::string(stack_buffer, count);
    }

    return message;
}

inline std::string Logger::ToUTF8(const std::wstring& text_utf16) {
    std::string text_utf8;

    enum { DEFAULT_SIZE = 4096 };
    char stack_buffer[DEFAULT_SIZE] = {};

    if (!text_utf16.empty()) {
        int size = WideCharToMultiByte(CP_UTF8, 0, text_utf16.c_str(), -1, NULL, 0, NULL, NULL);
        if (size == 0) {
            InnerFatalError("ToUTF8 Error: Can not convert a text from utf-16 to utf-8.");
        }

        char* buffer = (size > DEFAULT_SIZE) ? (new char[size]) : stack_buffer;

        size = WideCharToMultiByte(CP_UTF8, 0, text_utf16.c_str(), -1, buffer, size, NULL, NULL);
        if (size == 0) {
            InnerFatalError("ToUTF8 Error: Can not convert a text from utf-16 to utf-8.");
        }

        if (size > 0) text_utf8 = std::string(buffer, size - 1);

        if (buffer != stack_buffer) delete[] buffer;
    }

    return text_utf8;
}

//------------------------------------------------------------------------------

inline Logger::Tracker::Tracker(Logger& logger, const char* funcName) : m_logger(logger), m_function_name(funcName) {
    logger.LogTrace(funcName, "Enter.");
}

inline Logger::Tracker::~Tracker() {
    m_logger.LogTrace(m_function_name, "Exit.");
}

#endif // LOGGER_H_
