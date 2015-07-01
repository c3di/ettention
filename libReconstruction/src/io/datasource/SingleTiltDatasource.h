#pragma once
#include "model/geometry/ScannerGeometry.h"

namespace ettention
{
    class SingleTiltDatasource
    {
    public:
        SingleTiltDatasource();
        virtual ~SingleTiltDatasource();

        virtual ScannerGeometry getTiltGeometry(float tiltAngleInDegree, float xAxisTiltAngleInDegree) = 0;
    };
}