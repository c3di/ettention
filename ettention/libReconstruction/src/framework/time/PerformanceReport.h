#pragma once
#include "gpu/SingleInstanceTimestamps.h"

namespace ettention
{
    class PerformanceReport
    {
    public:
        PerformanceReport(const boost::filesystem::path& csvFilePath);
        ~PerformanceReport();

        void recordAction(const std::string& actionCaption, const SingleInstanceTimestamps& timestamps);
        void clearCurrentAttribute(const std::string& name);
        void setCurrentAttribute(const std::string& name, const std::string& value);

        template<typename _T>
        void setCurrentAttribute(const std::string& name, const _T& value);

    private:
        struct Entry 
        {
            std::vector<std::string> attributeValues;
            SingleInstanceTimestamps timestamps;
        };

        boost::filesystem::path csvFilePath;
        std::vector<std::string> attributeCaptions;
        std::vector<std::string> currentAttributeValues;
        std::vector<Entry> entries;
        std::ofstream csvFile;

        void openFile(bool keepContents);
        void closeFile();
        void fillFileWithHeader();
        void appendToCsvFile(const std::vector<std::string>& attributeValues, const SingleInstanceTimestamps& timestamps);
        void rewriteCsvFile();
    };

    template<typename _T>
    void PerformanceReport::setCurrentAttribute(const std::string& name, const _T& value)
    {
        std::stringstream str;
        str << value;
        setCurrentAttribute(name, str.str());
    }
}