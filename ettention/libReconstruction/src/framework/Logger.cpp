#include "stdafx.h"
#include "Logger.h"
#include <fstream>
#include <boost/log/expressions/attr.hpp>

namespace ettention
{
    Logger& Logger::getInstance()
    {
        static Logger logger;
        return logger;
    }

    Logger::Logger()
    {
        consoleLog = boost::shared_ptr<tSinkSyncOstream>(new tSinkSyncOstream);
        fileLog = boost::shared_ptr<tSinkSyncOstream>(new tSinkSyncOstream);
        addGlobalMessageAttributes();
        setupConsoleLog();
        setupFileLog();
    }

    Logger::~Logger()
    {

    }

    std::string Logger::getLogFilename() const
    {
        return fileLogName;
    }

    void Logger::setLogFilename(std::string fileLogName)
    {
        if( this->fileLogName.compare(fileLogName) == 0 )
            return;

        this->fileLogName = fileLogName;

        fileLogStream = boost::make_shared<std::ofstream>(this->fileLogName);
        if(!fileLogStream->good())
        {
            throw std::runtime_error("Failed to open a text log file");
        }
        fileLog->locked_backend()->add_stream(fileLogStream);
    }

    void Logger::setupConsoleLog()
    {
        boost::shared_ptr<std::ostream> consoleStream(&std::clog, boost::empty_deleter());
        consoleLog->locked_backend()->add_stream(consoleStream);
        useSimpleFormat(consoleLog);
    }

    void Logger::setupFileLog()
    {
        useSimpleFormat(fileLog);
    }

    void Logger::activateConsoleLog(FormatType format)
    {
        setFormat(consoleLog, format);
        boost::log::core::get()->add_sink(consoleLog);
    }

    void Logger::deactivateConsoleLog()
    {
        boost::log::core::get()->remove_sink(consoleLog);
    }

    void Logger::setConsoleLogLevel(SeverityLevel level)
    {
        consoleLog->set_filter(boost::log::expressions::attr<SeverityLevel>("Severity") >= warning);
    }

    void Logger::forceLogFlush()
    {
        if( fileLogStream )
            fileLogStream->flush();
    }

    void Logger::activateFileLog(FormatType format)
    {
        setFormat(fileLog, format);
        boost::log::core::get()->add_sink(fileLog);
    }

    void Logger::deactivateFileLog()
    {
        boost::log::core::get()->remove_sink(fileLog);
    }

    void Logger::setFileLogLevel(SeverityLevel level)
    {
        fileLog->set_filter(boost::log::expressions::attr<SeverityLevel>("Severity") >= warning);
    }

    void Logger::useWideFormat(boost::shared_ptr<tSinkSyncOstream> sink)
    {
        namespace keywords = boost::log::keywords;

        sink->set_formatter(boost::log::expressions::format("%1%: [%2%] [%3%] - %4%") %
                            boost::log::expressions::attr<unsigned int>("Line #") %
                            boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S") %
                            boost::log::expressions::attr<boost::thread::id>("ThreadID") %
                            boost::log::expressions::message);
    }

    void Logger::useSimpleFormat(boost::shared_ptr<tSinkSyncOstream> sink)
    {
        sink->set_formatter(boost::log::expressions::format("%1%") % boost::log::expressions::message);
    }

    void Logger::setFormat(boost::shared_ptr<tSinkSyncOstream> sink, FormatType format)
    {
        switch(format)
        {
        case FORMAT_WIDE:
            useWideFormat(sink);
            break;
        case FORMAT_SIMPLE:
        default:
            useSimpleFormat(sink);
            break;
        }
    }

    void Logger::addGlobalMessageAttributes()
    {
        using boost::log::attribute;
        using boost::log::attributes::local_clock;
        using boost::log::attributes::counter;
        using boost::log::attributes::current_thread_id;
        using boost::log::core;

        core::get()->add_global_attribute("TimeStamp", local_clock());

        core::get()->add_global_attribute("Line #", counter<unsigned int>());

        core::get()->add_global_attribute("ThreadID", current_thread_id());
    }

    // in order to detect memory leaks, the logger has to be initialized before the gtests start.
    // this class will enforce the creation of the logger right after the program starts by calling Logger::getInstance().
    // an alternative is to call Logger::getInstance() in the main method, which can easily be forgotten.
    class LoggerCreationEnforcer
    {
    public:
        LoggerCreationEnforcer()
        {
            Logger::getInstance();
        }

        ~LoggerCreationEnforcer()
        {
        }

    };

    static LoggerCreationEnforcer enforcer;
}