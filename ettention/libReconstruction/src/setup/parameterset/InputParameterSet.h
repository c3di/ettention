#pragma once
#include "setup/parameterset/ParameterSet.h"
#include "io/HyperStackIndex.h"

namespace ettention
{
    class ParameterSource;

    class InputParameterSet : public ParameterSet
    {
    public:
        InputParameterSet(const ParameterSource* parameterSource);
        ~InputParameterSet();

        const std::set<HyperStackIndex>& getSkipProjectionList() const;
        const boost::filesystem::path& getProjectionsPath() const;
        const boost::filesystem::path& getTiltAnglesPath() const;
        float getXAxisTilt() const;
        bool getLogarithmize() const;

    private:
        void parseSkipProjectionList(const std::string &s);
        void insertRangeIntoSkipProjectionsList(const std::string &s);

        std::set<HyperStackIndex> skipProjectionList;
        boost::filesystem::path projectionsPath;
        boost::filesystem::path tiltAnglesPath;
        float xAxisTilt;
        bool logarithmize;
    };
}