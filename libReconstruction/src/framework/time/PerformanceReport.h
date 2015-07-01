#pragma once
#include "gpu/SingleInstanceTimestamps.h"

namespace ettention
{
    class PerformanceReport
    {
    public:
        PerformanceReport();
        ~PerformanceReport();

        void recordAction(const std::string& actionCaption, const SingleInstanceTimestamps& timestamps);
        void clearCurrentAttribute(const std::string& name);
        void createCsvReport(const std::string& file) const;
        void clear();

        template<typename _T>
        void setCurrentAttribute(const std::string& name, const _T& value);

    private:
        typedef std::unordered_map<std::string, std::string> Attributes;
        struct Entry 
        {
            SingleInstanceTimestamps timestamps;
            Attributes attributes;
        };
        
        Attributes currentAttributes;
        std::vector<Entry> entries;
        std::unordered_set<std::string> usedAttributes;
    };

    template<typename _T>
    void PerformanceReport::setCurrentAttribute(const std::string& name, const _T& value)
    {
        std::stringstream str;
        str << value;
        currentAttributes[name] = str.str();
    }
}