#pragma once
#include "model/geometry/ScannerGeometry.h"

namespace ettention
{
    class AlgebraicParameterSet;

    class SingleTiltDatasource
    {
    public:
        SingleTiltDatasource();
        virtual ~SingleTiltDatasource();

        virtual ScannerGeometry* createTiltGeometry(float tiltAngleInDegree, AlgebraicParameterSet* paramSet) = 0;
    };
}