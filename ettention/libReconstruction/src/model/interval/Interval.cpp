#include "stdafx.h"
#include "Interval.h"

ettention::Interval::Interval(float outOfBoundValue)
: outOfBoundValue(outOfBoundValue)
{
}

float ettention::Interval::getOutOfBoundValue()
{
    return outOfBoundValue;
}
