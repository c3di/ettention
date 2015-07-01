#include "stdafx.h"

#include "framework/time/TimeUnits.h"

namespace TimeUnits
{
    ///////////////////////////////
    // TimeUnit
    ///////////////////////////////

    double TimeUnit::Value() const
    {
        return value;
    }

    void TimeUnit::Value(double newValue)
    {
        value = newValue;
    }    

    std::ostream& operator<< (std::ostream& out, const TimeUnit& timeUnit)
    {
        timeUnit.print(out);
        return out;
    }

    ///////////////////////////////
    // Second
    ///////////////////////////////

    Second::Second()        
    {
        Value(0.0);
    }

    Second::Second(double value)
    {
        Value(value);
    }

    Second Second::operator+(const Second& another)
    {
        return static_cast<Second>(Value() + another.Value());
    }

    Second Second::operator-(const Second& another)
    {
        return static_cast<Second>(Value() - another.Value());
    }

    Second Second::operator*(const Second& another)
    {
        return static_cast<Second>(Value() * another.Value());
    }

    Second Second::operator/(const Second& another)
    {
        return static_cast<Second>(Value() / another.Value());
    }

    template<>
    Millisecond Second::To()
    {
        return static_cast<Millisecond>(Value() * 1000.0);
    }

    template<>
    Microsecond Second::To()
    {
        return static_cast<Microsecond>(Value() * 1000000.0);
    }

    template<>
    Nanosecond  Second::To()
    {
        return static_cast<Nanosecond>(Value() * 1000000000.0);
    }

    void Second::print(std::ostream& stream) const
    {
        stream << Value() << " second(s)";
    }    

    ///////////////////////////////
    // Millisecond
    ///////////////////////////////

    Millisecond::Millisecond()        
    {
        Value(0.0);
    }

    Millisecond::Millisecond(double value)
    {
        Value(value);
    }

    Millisecond Millisecond::operator+(const Millisecond& another)
    {
        return static_cast<Millisecond>(Value() + another.Value());
    }

    Millisecond Millisecond::operator-(const Millisecond& another)
    {
        return static_cast<Millisecond>(Value() - another.Value());
    }

    Millisecond Millisecond::operator*(const Millisecond& another)
    {
        return static_cast<Millisecond>(Value() * another.Value());
    }

    Millisecond Millisecond::operator/(const Millisecond& another)
    {
        return static_cast<Millisecond>(Value() / another.Value());
    }

    template<>
    Second Millisecond::To()
    {
        return static_cast<Second>(Value() / 1000.0);
    }

    template<>
    Microsecond Millisecond::To()
    {
        return static_cast<Microsecond>(Value() * 1000.0);
    }

    template<>
    Nanosecond  Millisecond::To()
    {
        return static_cast<Nanosecond>(Value() * 1000000.0);
    }

    void Millisecond::print(std::ostream& stream) const
    {
        stream << Value() << " millisecond(s)";
    }  

    ///////////////////////////////
    // Microsecond
    ///////////////////////////////

    Microsecond::Microsecond()        
    {
        Value(0.0);
    }

    Microsecond::Microsecond(double value)
    {
        Value(value);
    }

    Microsecond Microsecond::operator+(const Microsecond& another)
    {
        return static_cast<Microsecond>(Value() + another.Value());
    }

    Microsecond Microsecond::operator-(const Microsecond& another)
    {
        return static_cast<Microsecond>(Value() - another.Value());
    }

    Microsecond Microsecond::operator*(const Microsecond& another)
    {
        return static_cast<Microsecond>(Value() * another.Value());
    }

    Microsecond Microsecond::operator/(const Microsecond& another)
    {
        return static_cast<Microsecond>(Value() / another.Value());
    }

    template<>
    Second Microsecond::To()
    {
        return static_cast<Second>(Value() / 1000000.0);
    }

    template<>
    Millisecond Microsecond::To()
    {
        return static_cast<Millisecond>(Value() / 1000.0);
    }

    template<>
    Nanosecond  Microsecond::To()
    {
        return static_cast<Nanosecond>(Value() * 1000.0);
    }

    void Microsecond::print(std::ostream& stream) const
    {
        stream << Value() << " microsecond(s)";
    }  

    ///////////////////////////////
    // Nanosecond
    ///////////////////////////////

    Nanosecond::Nanosecond()        
    {
        Value(0.0);
    }

    Nanosecond::Nanosecond(double value)
    {
        Value(value);
    }

    Nanosecond Nanosecond::operator+(const Nanosecond& another)
    {
        return static_cast<Nanosecond>(Value() + another.Value());
    }

    Nanosecond Nanosecond::operator-(const Nanosecond& another)
    {
        return static_cast<Nanosecond>(Value() - another.Value());
    }

    Nanosecond Nanosecond::operator*(const Nanosecond& another)
    {
        return static_cast<Nanosecond>(Value() * another.Value());
    }

    Nanosecond Nanosecond::operator/(const Nanosecond& another)
    {
        return static_cast<Nanosecond>(Value() / another.Value());
    }

    template<>
    Second Nanosecond::To()
    {
        return static_cast<Second>(Value() / 1000000000.0);
    }

    template<>
    Millisecond Nanosecond::To()
    {
        return static_cast<Millisecond>(Value() / 1000000.0);
    }

    template<>
    Microsecond  Nanosecond::To()
    {
        return static_cast<Microsecond>(Value() / 1000.0);
    }

    void Nanosecond::print(std::ostream& stream) const
    {
        stream << Value() << " nanosecond(s)";
    }  

}
