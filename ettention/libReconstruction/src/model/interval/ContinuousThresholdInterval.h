#pragma once
#include "Interval.h"

namespace ettention
{
    class ContinuousThresholdInterval : public Interval
    {

    public:
        ContinuousThresholdInterval();

        virtual void parseFromString(const std::string& input) override;
        virtual std::string convertToString() const override;

        void parseIntensityValues(const std::string& input);

        const std::vector<float>& getThresholdValues() const;
        const std::vector<float>& getIntensityValues() const;

    private:
        std::vector<float> levelThresholdValues;
        std::vector<float> levelIntensityValues;
    };
}