#pragma once
#include "stdafx.h"
BOOST_LOG_INLINE_GLOBAL_LOGGER_DEFAULT(ettentionSeverityLogger, boost::log::sources::logger_mt)

namespace ettention
{
    class Logger
    {
    public:
        enum FormatType
        {
            FORMAT_SIMPLE,
            FORMAT_WIDE
        };

        enum SeverityLevel
        {
            normal,
            warning,
            debug,
            important,
            error
        };

        static const int FillLength = 80;

        static Logger& getInstance();

        std::string getLogFilename() const;
        void setLogFilename(std::string val);
        void activateConsoleLog(FormatType format = FORMAT_SIMPLE);
        void deactivateConsoleLog();
        void setConsoleLogLevel(SeverityLevel level);
        void activateFileLog(FormatType format = FORMAT_SIMPLE);
        void deactivateFileLog();
        void setFileLogLevel(SeverityLevel level);
        void forceLogFlush();

    private:
        typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > tSinkSyncOstream;

        Logger();
        ~Logger();

        void setupConsoleLog();
        void setupFileLog();
        void addGlobalMessageAttributes();
        void useWideFormat(boost::shared_ptr<tSinkSyncOstream> sink);
        void useSimpleFormat(boost::shared_ptr<tSinkSyncOstream> sink);
        void setFormat(boost::shared_ptr<tSinkSyncOstream> sink, FormatType format);

        boost::shared_ptr<tSinkSyncOstream> consoleLog;
        boost::shared_ptr<tSinkSyncOstream> fileLog;
        boost::shared_ptr< std::ostream > fileLogStream;
        std::string fileLogName;
    };

    template< typename CharT, typename TraitsT >
    inline std::basic_ostream<CharT, TraitsT>& operator<< (std::basic_ostream<CharT, TraitsT>& strm, Logger::SeverityLevel lvl)
    {
        static const char* const str[] =
        {
            "normal",
            "notification",
            "warning",
            "error",
            "critical",
        };
        if(static_cast<std::size_t>(lvl) < (sizeof(str) / sizeof(*str)))
        {
            strm << str[lvl];
        }
        else
        {
            strm << static_cast<int>(lvl);
        }
        return strm;
    }

#define LOGGER_NEW_LINE()\
    LOGGER("\n")

#define LOGGER(message)\
    BOOST_LOG_SEV(ettentionSeverityLogger::get(), Logger::normal) << message

#define LOGGER_IMP(message)\
    BOOST_LOG_SEV(ettentionSeverityLogger::get(), Logger::important) << message

#define LOGGER_ERROR(message)\
    BOOST_LOG_SEV(ettentionSeverityLogger::get(), Logger::error) << message << "  (File: " << __FILE__ << ", line: " << __LINE__ << ")";

#define LOGGER_INFO_FORMATTED(message)\
    BOOST_LOG_SEV(ettentionSeverityLogger::get(), Logger::normal) << std::setw(Logger::FillLength) << std::setfill('.') << std::left << message

#define LOGGER_FILE_ERROR(name)\
    BOOST_LOG_SEV(ettentionSeverityLogger::get(), Logger::error) << "Unable to open file: " << name << "  (File: " << __FILE__ << ", line: " << __LINE__ << ")";

#define LOGGER_TAG(name)\
    BOOST_LOG_SCOPED_LOGGER_TAG(ettentionSeverityLogger::get(), "Tag", std::string, name)

}
