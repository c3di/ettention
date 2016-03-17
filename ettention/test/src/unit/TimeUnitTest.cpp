#include "stdafx.h"
#include "framework/time/TimeUnits.h"

namespace ettention
{
    class TimeUnitTest : public TestBase
    {
    public:
        TimeUnitTest() :
            anyPositiveFloat(1.f)
        {
        }

    protected:
        const float anyPositiveFloat;
    };
}

using namespace ettention;
using namespace TimeUnits;

TEST_F(TimeUnitTest, Unit_Positive_Second_ArithmeticOperators)
{
    Second measurementA(anyPositiveFloat);
    Second measurementB(anyPositiveFloat);
    Second sumOfMeasurements;

    sumOfMeasurements = measurementA + measurementB;
    ASSERT_TRUE(sumOfMeasurements.Value() == (anyPositiveFloat + anyPositiveFloat));
    sumOfMeasurements = measurementA - measurementB;
    ASSERT_TRUE(sumOfMeasurements.Value() == (anyPositiveFloat - anyPositiveFloat));
    sumOfMeasurements = measurementA * measurementB;
    ASSERT_TRUE(sumOfMeasurements.Value() == (anyPositiveFloat * anyPositiveFloat));
    sumOfMeasurements = measurementA / measurementB;
    ASSERT_TRUE(sumOfMeasurements.Value() == (anyPositiveFloat / anyPositiveFloat));
}

TEST_F(TimeUnitTest, Unit_Positive_Millisecond_ArithmeticOperators)
{
    Millisecond measurementA(anyPositiveFloat);
    Millisecond measurementB(anyPositiveFloat);
    Millisecond sumOfMeasurements;

    sumOfMeasurements = measurementA + measurementB;
    ASSERT_TRUE(sumOfMeasurements.Value() == (anyPositiveFloat + anyPositiveFloat));
    sumOfMeasurements = measurementA - measurementB;
    ASSERT_TRUE(sumOfMeasurements.Value() == (anyPositiveFloat - anyPositiveFloat));
    sumOfMeasurements = measurementA * measurementB;
    ASSERT_TRUE(sumOfMeasurements.Value() == (anyPositiveFloat * anyPositiveFloat));
    sumOfMeasurements = measurementA / measurementB;
    ASSERT_TRUE(sumOfMeasurements.Value() == (anyPositiveFloat / anyPositiveFloat));
}