#include "stdafx.h"
#include "SingleInstanceTimestamps.h"
#include "framework/error/Exception.h"

namespace ettention
{
    SingleInstanceTimestamps::SingleInstanceTimestamps()
        : queued(0)
        , submitted(0)
        , started(0)
        , finished(0)
    {
    }

    SingleInstanceTimestamps::SingleInstanceTimestamps(unsigned long long queued, unsigned long long submitted, unsigned long long started, unsigned long long finished)
        : queued(queued)
        , submitted(submitted)
        , started(started)
        , finished(finished)
    {
    }

    SingleInstanceTimestamps::~SingleInstanceTimestamps()
    {
    }

    unsigned long long SingleInstanceTimestamps::GetTimestampOf(Action action) const
    {
        switch(action)
        {
        case ACTION_QUEUED:
            return queued;
        case ACTION_SUBMITTED:
            return submitted;
        case ACTION_STARTED:
            return started;
        case ACTION_FINISHED:
            return finished;
        default:
            throw Exception("Illegal action value!");
        }
    }

    double SingleInstanceTimestamps::GetElapsedTime(Action fromAction, Action toAction, Unit unit) const
    {
        auto fromTimestamp = this->GetTimestampOf(fromAction);
        auto toTimestamp = this->GetTimestampOf(toAction);
        if(fromTimestamp > toTimestamp)
        {
            throw Exception("FromAction lies behind ToAction!");
        }
        auto nanos = toTimestamp - fromTimestamp;
        switch(unit)    // always provide 3 digits after the decimal point (except for nanoseconds)
        {
        case UNIT_NANOSECONDS:
            return (double)nanos;
        case UNIT_MICROSECONDS:
            return 1e-3 * (double)nanos;
        case UNIT_MILLISECONDS:
            return 1e-3 * (double)(nanos / 1000);
        case UNIT_SECONDS:
            return 1e-3 * (double)(nanos / 1000000);
        default:
            throw Exception("Unknown time unit!");
        }
    }

    std::ostream& operator<<(std::ostream& out, const KernelLifetimeTimings& timings)
    {
        out << "Instance;Enqueue to submission (ms);Submission to start (ms);HW execution time (ms)" << std::endl;
        for(unsigned int i = 0; i < timings.size(); ++i)
        {
            out << i << ";" << timings[i].GetElapsedTime(SingleInstanceTimestamps::ACTION_QUEUED, SingleInstanceTimestamps::ACTION_SUBMITTED, SingleInstanceTimestamps::UNIT_MILLISECONDS);
            out << ";" << timings[i].GetElapsedTime(SingleInstanceTimestamps::ACTION_SUBMITTED, SingleInstanceTimestamps::ACTION_STARTED, SingleInstanceTimestamps::UNIT_MILLISECONDS);
            out << ";" << timings[i].GetElapsedTime(SingleInstanceTimestamps::ACTION_STARTED, SingleInstanceTimestamps::ACTION_FINISHED, SingleInstanceTimestamps::UNIT_MILLISECONDS) << std::endl;
        }
        return out;
    }
}