#include "Logger.h"
#include "ino_compat.h"

namespace mini_lcd
{
Logger& Logger::GetLogger()
{
    static Logger instance;
    return instance;
}

void Logger::SetVerbosity(Verbosity verbosity)
{
    verbosity_ = verbosity;
}

Logger& Logger::trace(std::source_location loc)
{
    return start(loc, Verbosity::Trace);
}

Logger& Logger::debug(std::source_location loc)
{
    return start(loc, Verbosity::Debug);
}

Logger& Logger::info(std::source_location loc)
{
    return start(loc, Verbosity::Info);
}

Logger& Logger::warn(std::source_location loc)
{
    return start(loc, Verbosity::Warn);
}

Logger& Logger::error(std::source_location loc)
{
    return start(loc, Verbosity::Error);
}

Logger& Logger::start(std::source_location& loc, Verbosity verbosity)
{
    auto& logger = GetLogger();
    if (logger.verbosity_ <= verbosity) {
        std::cout << millis() << " [" << verbosityToString(verbosity) << "]" << loc.file_name()
                  << ":" << loc.line() << " ";
    }
    logger.lineVerbosity = verbosity;
    return logger;
}

const char* Logger::verbosityToString(Verbosity verbosity)
{
    switch (verbosity) {
        case Verbosity::Trace:
            return "TRACE";
        case Verbosity::Debug:
            return "DEBUG";
        case Verbosity::Info:
            return "INFO";
        case Verbosity::Warn:
            return "WARN";
        case Verbosity::Error:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

} // namespace mini_lcd
