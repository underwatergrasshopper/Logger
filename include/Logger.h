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
* @version 2.0.1
*/

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <locale.h>

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN 

#include <string>
#include <utility>

//------------------------------------------------------------------------------
// Declaration
//------------------------------------------------------------------------------

enum class LoggerOption {
    LOG_TIME,
    LOG_TRACE,
    LOG_DUMP,
    LOG_EVENT,
    LOG_WARNING
};

class Logger {
public:
    typedef void (*DoAtFatalErrorFnP_T)(const char* message);

    Logger();
    virtual ~Logger();

    // Opens or creates a log file to be logged in.
    // file_name            Name of log file. Encoding: ACII or UTF8.
    // is_append            If true then current content of file is keept. 
    //                      If false then current content of file is removed.
    void OpenFile(const std::string& file_name, bool is_append);

    // Closes log file.
    void CloseFile();

    bool IsFileOpened() const;

    // Enables redirecting log messages to standard output.
    void OpenStdOut();

    // Disables redirecting log messages to standard output.
    void CloseStdOut();

    bool IsStdOutOpened() const;

    void SetDoAtFatalError(DoAtFatalErrorFnP_T do_at_fatal_error);

    // Enables/Disables:
    //      LOG_TIME    - logging with timestamps (default: Disabled),
    //      LOG_TRACE   - logging trace (default: Enabled),
    //      LOG_DUMP    - logging dump (default: Enabled),
    //      LOG_EVENT   - logging event (default: Enabled),
    //      LOG_WARNING - logging warning (default: Enabled).
    void Enable(LoggerOption option);
    void Disable(LoggerOption option);
    void SetOption(LoggerOption option, bool is);

    // for all Log{...}() methods:
    // Expected string encoding: ASCII or UTF8.
    // format           The format of a log message, same rules as in standard 'printf' function.
    // arguments        The arguments interpreted by format, same rules as in standard 'printf' function.
    // function_name    The name of the function or method where Log{...}() is called.

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

    // Will exit from program after logging fatal error message.
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
    class UTF8_Guardian {
    public:
        UTF8_Guardian() {
            char* current = setlocale(LC_ALL, ".UTF8");
            backup = current ? std::string(current) : "";
        }
        virtual ~UTF8_Guardian() {
            setlocale(LC_ALL, backup.c_str());
        }
    private:
        std::string backup;
    };

    template <typename... Types>
    void LogEntry(const std::string& category_name, const std::string& format, Types&&... arguments);

    void LogTime();

    void InnerFatalError(const char* message, const wchar_t* message_utf16);

    template <typename... Types>
    std::string GenerateMessage(const std::string& format, Types&&... arguments);

    std::wstring ToUTF16(const std::string& text_utf8);

    FILE*                   m_file;
    bool                    m_is_stdout;
    DoAtFatalErrorFnP_T     m_do_at_fatal_error;

    bool                    m_is_log_time;
    bool                    m_is_log_trace;
    bool                    m_is_log_dump;
    bool                    m_is_log_event;
    bool                    m_is_log_warning;
};

#define LOGGER_TRACE(logger, ...) logger.LogTrace(__FUNCTION__ , __VA_ARGS__)
#define LOGGER_TRACK(logger) Logger::Tracker l_tracker(logger, __FUNCTION__)

//------------------------------------------------------------------------------
// Definition
//------------------------------------------------------------------------------

#define TOSTR_INNER_FATAL_ERROR(message) InnerFatalError(message, L##message)

inline Logger::Logger() {
    m_file              = nullptr;
    m_is_stdout         = false;
    m_do_at_fatal_error = nullptr;

    m_is_log_time       = false;
    m_is_log_trace      = true;
    m_is_log_dump       = true;
    m_is_log_event      = true;
    m_is_log_warning    = true;
}

inline Logger::~Logger() {
    CloseFile();
}

//------------------------------------------------------------------------------

inline void Logger::OpenFile(const std::string& file_name, bool is_append) {
    CloseFile();
    if (_wfopen_s(&m_file, ToUTF16(file_name).c_str(), is_append ? L"ab" : L"wb") != 0 || !m_file) {
        TOSTR_INNER_FATAL_ERROR("Error Logger::OpenFile: Can not open log file.");
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
    SetOption(option, true);
}
inline void Logger::Disable(LoggerOption option) {
    SetOption(option, false);
}

inline void Logger::SetOption(LoggerOption option, bool is) {
    switch (option) {
        case LoggerOption::LOG_TIME:    m_is_log_time       = is; break;
        case LoggerOption::LOG_TRACE:   m_is_log_trace      = is; break;
        case LoggerOption::LOG_DUMP:    m_is_log_dump       = is; break;
        case LoggerOption::LOG_EVENT:   m_is_log_event      = is; break;
        case LoggerOption::LOG_WARNING: m_is_log_warning    = is; break;
    }
}

//------------------------------------------------------------------------------

inline void Logger::LogText(const std::string& text) {
    if (m_file) {
        const size_t count = fwrite(text.c_str(), sizeof(char), text.length(), m_file);
        if (count != text.length()) {
            TOSTR_INNER_FATAL_ERROR("Logger::LogText: Failed write the text to the log file.");
        }

        fflush(m_file);
    }
    if (m_is_stdout) {
        UTF8_Guardian utf8_guardian;

        if (fwide(stdout, 0) > 0) {
            wprintf(L"%ls", ToUTF16(text).c_str());
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
    if (m_is_log_trace) {
        if (m_is_log_time) LogTime();
        LogText("[Trace][%s]: ", function_name.c_str());
        LogText(format, std::forward<Types>(arguments)...);
        LogText("\n");
    }
}
template <typename... Types>
void Logger::LogDump(const std::string& format, Types&&... arguments) {
    if (m_is_log_dump) LogEntry("Dump", format, std::forward<Types>(arguments)...);
} 

template <typename... Types>
void Logger::LogEvent(const std::string& format, Types&&... arguments) {
    if (m_is_log_event) LogEntry("Event", format, std::forward<Types>(arguments)...);
} 

template <typename... Types>
void Logger::LogWarning(const std::string& format, Types&&... arguments) {
    if (m_is_log_warning) LogEntry("Warning", format, std::forward<Types>(arguments)...);
} 

template <typename... Types>
void Logger::LogError(const std::string& format, Types&&... arguments) {
    LogEntry("Error", format, std::forward<Types>(arguments)...);
} 

template <typename... Types>
void Logger::LogFatalError(const std::string& format, Types&&... arguments) {
    const std::string message = GenerateMessage(format, std::forward<Types>(arguments)...);
    LogEntry("Fatal Error", message);

    if (m_do_at_fatal_error) m_do_at_fatal_error(message.c_str()); 
    exit(EXIT_FAILURE);
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

inline void Logger::InnerFatalError(const char* message, const wchar_t* message_utf16) {
    {
        UTF8_Guardian utf8_guardian;

        if (fwide(stdout, 0) > 0) {
            wprintf(L"%ls\n", message_utf16);
        } else {
            printf("%s\n", message);
        }
        fflush(stdout);
    }

    if (m_do_at_fatal_error) m_do_at_fatal_error(message);
    exit(EXIT_FAILURE);
}

template <typename... Types>
std::string Logger::GenerateMessage(const std::string& format, Types&&... arguments) {
    std::string message;

    enum { SIZE = 4096 };
    char stack_buffer[SIZE];

    #ifndef _MSC_VER 
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat-security"
    #endif

    const int count = snprintf(stack_buffer, SIZE, format.c_str(), std::forward<Types>(arguments)...);

    #ifndef _MSC_VER 
        #pragma GCC diagnostic pop
    #endif

    if (count < 0) {
        TOSTR_INNER_FATAL_ERROR("Error Logger::GenerateMessage: Wrong encoding.");
    }

    if (count >= SIZE) {
        char* buffer = new char[count];

        #ifndef _MSC_VER 
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wformat-security"
        #endif

        const int second_count = snprintf(buffer, count, format.c_str(), std::forward<Types>(arguments)...);

        #ifndef _MSC_VER 
            #pragma GCC diagnostic pop
        #endif

        if (count < 0) {
            TOSTR_INNER_FATAL_ERROR("Error Logger::GenerateMessage: Wrong encoding (at second try).");
        }
        if (second_count > count) {
            TOSTR_INNER_FATAL_ERROR("Error Logger::GenerateMessage: Can not write to buffer.");
        }

        if (second_count > 0) message = std::string(buffer, second_count);

        delete[] buffer;
    } else {
        message = std::string(stack_buffer, count);
    }

    return message;
}

inline std::wstring Logger::ToUTF16(const std::string& text_utf8) {
    std::wstring text_utf16;

    enum { DEFAULT_SIZE = 4096 };
    wchar_t stack_buffer[DEFAULT_SIZE] = {};

    if (!text_utf8.empty()) {
        int size = MultiByteToWideChar(CP_UTF8, 0, text_utf8.c_str(), -1, NULL, 0);
        if (size == 0) {
            TOSTR_INNER_FATAL_ERROR("Error Logger::ToUTF16: Can not convert a text from utf-16 to utf-8.");
        }

        wchar_t* buffer = (size > DEFAULT_SIZE) ? (new wchar_t[size]) : stack_buffer;

        size = MultiByteToWideChar(CP_UTF8, 0, text_utf8.c_str(), -1, buffer, size);
        if (size == 0) {
            TOSTR_INNER_FATAL_ERROR("Error Logger::ToUTF16: Can not convert a text from utf-16 to utf-8.");
        }

        if (size > 1) text_utf16 = std::wstring(buffer, size - 1);

        if (buffer != stack_buffer) delete[] buffer;
    }

    return text_utf16;
}

//------------------------------------------------------------------------------

inline Logger::Tracker::Tracker(Logger& logger, const char* function_name) : m_logger(logger), m_function_name(function_name) {
    logger.LogTrace(function_name, "Enter.");
}

inline Logger::Tracker::~Tracker() {
    m_logger.LogTrace(m_function_name, "Exit.");
}

#endif // LOGGER_H_
