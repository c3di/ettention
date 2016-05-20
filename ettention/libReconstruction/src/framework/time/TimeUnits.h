#pragma once

#include <iostream>

namespace TimeUnits
{    
    class Second;
    class Millisecond;
    class Microsecond;
    class Nanosecond;

    ///////////////////////////////
    // Generic TimeUnit
    ///////////////////////////////
    class TimeUnit
    {
    public:        

        double Value() const;
        void Value(double newValue);

    protected:
        virtual void print(std::ostream& stream) const = 0;
        double value;

    private:
        friend std::ostream& operator<< (std::ostream& out, const TimeUnit& timeUnit);
    };

    std::ostream& operator<< (std::ostream& out, const TimeUnit& timeUnit);

    ///////////////////////////////
    // Second
    ///////////////////////////////
    class Second : public TimeUnit
    {
    public:

        Second();
        explicit Second(double value);

        Second operator + (const Second& another); 
        Second operator - (const Second& another);
        Second operator * (const Second& another);
        Second operator / (const Second& another);

        template<typename TimeUnitType>
        TimeUnitType To();

    protected:
        virtual void print(std::ostream& stream) const override;

    };

    ///////////////////////////////
    // Millisecond
    ///////////////////////////////
    class Millisecond : public TimeUnit
    {
    public:

        Millisecond();
        explicit Millisecond(double value);

        Millisecond operator + (const Millisecond& another); 
        Millisecond operator - (const Millisecond& another);
        Millisecond operator * (const Millisecond& another);
        Millisecond operator / (const Millisecond& another);

        template<typename TimeUnitType>
        TimeUnitType To();

    protected:
        virtual void print(std::ostream& stream) const override;
    };


    ///////////////////////////////
    // Microsecond
    ///////////////////////////////
    class Microsecond : public TimeUnit
    {
    public:

        Microsecond();
        explicit Microsecond(double value);

        Microsecond operator + (const Microsecond& another); 
        Microsecond operator - (const Microsecond& another);
        Microsecond operator * (const Microsecond& another);
        Microsecond operator / (const Microsecond& another);

        template<typename TimeUnitType>
        TimeUnitType To();

    protected:
        virtual void print(std::ostream& stream) const override;
    };

    ///////////////////////////////
    // Nanosecond
    ///////////////////////////////
    class Nanosecond : public TimeUnit
    {
    public:

        Nanosecond();
        explicit Nanosecond(double value);

        Nanosecond operator + (const Nanosecond& another); 
        Nanosecond operator - (const Nanosecond& another);
        Nanosecond operator * (const Nanosecond& another);
        Nanosecond operator / (const Nanosecond& another);

        template<typename TimeUnitType>
        TimeUnitType To();

    protected:
        virtual void print(std::ostream& stream) const override;
    };

    //////////////////////////////////////
    // Explicit template specializations
    //////////////////////////////////////
    template<>
    Millisecond Second::To();    
    template<>
    Microsecond Second::To();
    template<>
    Nanosecond  Second::To();

    template<>
    Second      Millisecond::To();    
    template<>
    Microsecond Millisecond::To();
    template<>
    Nanosecond  Millisecond::To();

    template<>
    Second      Microsecond::To();    
    template<>
    Millisecond Microsecond::To();
    template<>
    Nanosecond  Microsecond::To();

    template<>
    Second      Nanosecond::To();    
    template<>
    Millisecond Nanosecond::To();
    template<>
    Microsecond  Nanosecond::To();

}