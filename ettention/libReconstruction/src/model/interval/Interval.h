#pragma once

namespace ettention
{
    class Interval
    {

    public:
        Interval(float outOfBoundValue = -1.0f);

        virtual void parseFromString(const std::string& input) = 0;
        virtual std::string convertToString() const = 0;

        float getOutOfBoundValue();

    private:
        float outOfBoundValue;
    };
}