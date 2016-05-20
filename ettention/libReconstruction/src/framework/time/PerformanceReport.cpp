#include "stdafx.h"
#include "PerformanceReport.h"
#include "framework/error/Exception.h"
#include "framework/Logger.h"

namespace ettention
{
    PerformanceReport::PerformanceReport(const boost::filesystem::path& csvFilePath)
        : csvFilePath(csvFilePath)
    {
    }

    PerformanceReport::~PerformanceReport()
    {
    }

    void PerformanceReport::recordAction(const std::string& actionCaption, const SingleInstanceTimestamps& timestamps)
    {
        setCurrentAttribute("Action", actionCaption);
        Entry entry = { currentAttributeValues, timestamps, };
        entries.push_back(entry);
        openFile(true);
        appendToCsvFile(currentAttributeValues, timestamps);
        closeFile();
        clearCurrentAttribute("Action");
    }

    void PerformanceReport::setCurrentAttribute(const std::string& name, const std::string& value)
    {
        for(unsigned int i = 0; i < attributeCaptions.size(); ++i)
        {
            if(attributeCaptions[i] == name)
            {
                currentAttributeValues[i] = value;
                return;
            }
        }
        attributeCaptions.push_back(name);
        currentAttributeValues.push_back(value);
        rewriteCsvFile();
    }

    void PerformanceReport::clearCurrentAttribute(const std::string& name)
    {
        for(unsigned int i = 0; i < attributeCaptions.size(); ++i)
        {
            if(attributeCaptions[i] == name)
            {
                currentAttributeValues[i].clear();
                return;
            }
        }
        LOGGER_ERROR("Tried to clear non-existing attribute " << name << " from PerformanceReport!");
    }

    void PerformanceReport::appendToCsvFile(const std::vector<std::string>& attributeValues, const SingleInstanceTimestamps& timestamps)
    {
        for(unsigned int i = 0; i < attributeCaptions.size(); ++i)
        {
            csvFile << (i < attributeValues.size() ? attributeValues[i] : "") << ";";
        }
        csvFile << timestamps.GetElapsedTime(SingleInstanceTimestamps::ACTION_QUEUED, SingleInstanceTimestamps::ACTION_SUBMITTED, SingleInstanceTimestamps::UNIT_MILLISECONDS) << ";";
        csvFile << timestamps.GetElapsedTime(SingleInstanceTimestamps::ACTION_SUBMITTED, SingleInstanceTimestamps::ACTION_STARTED, SingleInstanceTimestamps::UNIT_MILLISECONDS) << ";";
        csvFile << timestamps.GetElapsedTime(SingleInstanceTimestamps::ACTION_STARTED, SingleInstanceTimestamps::ACTION_FINISHED, SingleInstanceTimestamps::UNIT_MILLISECONDS) << ";" << std::endl;
    }

    void PerformanceReport::rewriteCsvFile()
    {
        openFile(false);
        fillFileWithHeader();
        for(auto it = entries.begin(); it != entries.end(); ++it)
        {
            appendToCsvFile(it->attributeValues, it->timestamps);
        }
        closeFile();
    }

    void PerformanceReport::fillFileWithHeader()
    {
        csvFile << "Performance report" << std::endl;
        for(auto it = attributeCaptions.begin(); it != attributeCaptions.end(); ++it)
        {
            csvFile << *it << ";";
        }
        csvFile << "Enqueue to submission (ms);Submission to start (ms);HW execution time (ms)" << std::endl;
    }

    void PerformanceReport::openFile(bool keepContents)
    {
        csvFile.open(csvFilePath.string(), keepContents ? std::ios::app : std::ios::trunc);
        if(!csvFile.is_open())
        {
            throw Exception("Failed to open " + csvFilePath.string() + " for csv report!");
        }
    }

    void PerformanceReport::closeFile()
    {
        csvFile.close();
    }
}