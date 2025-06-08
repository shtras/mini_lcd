#pragma once

#include <iostream>
#include <source_location>

namespace mini_lcd
{
class Logger
{
public:
    enum class Verbosity { Trace, Debug, Info, Warn, Error };
    static Logger& trace(std::source_location loc = std::source_location::current());
    static Logger& debug(std::source_location loc = std::source_location::current());
    static Logger& info(std::source_location loc = std::source_location::current());
    static Logger& warn(std::source_location loc = std::source_location::current());
    static Logger& error(std::source_location loc = std::source_location::current());
    static Logger& GetLogger();
    void SetVerbosity(Verbosity verbosity);

    template <typename T>
    Logger& operator<<(const T& message)
    {
        if (lineVerbosity >= verbosity_) {
            std::cout << message;
        }
        return *this;
    }

private:
    static const char* verbosityToString(Verbosity verbosity);
    static Logger& start(std::source_location& loc, Verbosity verbosity);
    Verbosity verbosity_ = Verbosity::Info;
    Verbosity lineVerbosity = Verbosity::Info;
};
} // namespace mini_lcd
