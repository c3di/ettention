#pragma once

namespace ettention
{
    class SingleInstanceTimestamps
    {
    public:
        enum Unit
        {
            UNIT_NANOSECONDS,
            UNIT_MICROSECONDS,
            UNIT_MILLISECONDS,
            UNIT_SECONDS,
        };
        enum Action
        {
            ACTION_QUEUED,
            ACTION_SUBMITTED,
            ACTION_STARTED,
            ACTION_FINISHED,
        };

        SingleInstanceTimestamps();
        SingleInstanceTimestamps(unsigned long long queued, unsigned long long submitted, unsigned long long started, unsigned long long finished);
        ~SingleInstanceTimestamps();

        double GetElapsedTime(Action fromAction, Action toAction, Unit unit) const;

    private:
        unsigned long long GetTimestampOf(Action action) const;

        unsigned long long queued;
        unsigned long long submitted;
        unsigned long long started;
        unsigned long long finished;
    };

    typedef std::vector<SingleInstanceTimestamps> KernelLifetimeTimings;
    std::ostream& operator<<(std::ostream& out, const KernelLifetimeTimings& timings);
}