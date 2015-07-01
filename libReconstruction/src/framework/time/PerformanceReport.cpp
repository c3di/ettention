#include "stdafx.h"
#include "PerformanceReport.h"
#include "framework/error/Exception.h"
#include "framework/Logger.h"

namespace ettention
{
    PerformanceReport::PerformanceReport()
    {

    }

    PerformanceReport::~PerformanceReport()
    {

    }

    void PerformanceReport::recordAction(const std::string& actionCaption, const SingleInstanceTimestamps& timestamps)
    {
        setCurrentAttribute("Action", actionCaption);
        Entry entry = { timestamps, currentAttributes, };
        entries.push_back(entry);
        for(auto it = currentAttributes.begin(); it != currentAttributes.end(); ++it)
        {
            usedAttributes.insert(it->first);
        }
        clearCurrentAttribute("Action");
    }

    void PerformanceReport::createCsvReport(const std::string& file) const
    {
        std::ofstream out(file);
        if(!out.is_open())
        {
            throw Exception("Failed to open " + file + " for csv report!");
        }
        out << "Performance report" << std::endl;
        for(auto it = usedAttributes.begin(); it != usedAttributes.end(); ++it)
        {
            out << *it << ";";
        }
        out << "Enqueue to submission (ms);Submission to start (ms);HW execution time (ms)" << std::endl;
        for(auto it = entries.begin(); it != entries.end(); ++it)
        {
            for(auto attrIt = usedAttributes.begin(); attrIt != usedAttributes.end(); ++attrIt)
            {
                auto findIt = it->attributes.find(*attrIt);
                out << (findIt == it->attributes.end() ? "" : findIt->second) << ";";
            }
            out << it->timestamps.GetElapsedTime(SingleInstanceTimestamps::ACTION_QUEUED, SingleInstanceTimestamps::ACTION_SUBMITTED, SingleInstanceTimestamps::UNIT_MILLISECONDS) << ";";
            out << it->timestamps.GetElapsedTime(SingleInstanceTimestamps::ACTION_SUBMITTED, SingleInstanceTimestamps::ACTION_STARTED, SingleInstanceTimestamps::UNIT_MILLISECONDS) << ";";
            out << it->timestamps.GetElapsedTime(SingleInstanceTimestamps::ACTION_STARTED, SingleInstanceTimestamps::ACTION_FINISHED, SingleInstanceTimestamps::UNIT_MILLISECONDS) << ";" << std::endl;
        }
        out.close();
    }

    void PerformanceReport::clearCurrentAttribute(const std::string& name)
    {
        auto findIt = currentAttributes.find(name);
        if(findIt != currentAttributes.end())
        {
            currentAttributes.erase(findIt);
        }
    }

    void PerformanceReport::clear()
    {
        currentAttributes.clear();
        entries.clear();
        usedAttributes.clear();
    }
}