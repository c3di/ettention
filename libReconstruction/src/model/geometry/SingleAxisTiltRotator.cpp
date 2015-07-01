#include "stdafx.h"
#include "SingleAxisTiltRotator.h"

namespace ettention
{
    SingleAxisTiltRotator::SingleAxisTiltRotator()
    {
    }

    SingleAxisTiltRotator::~SingleAxisTiltRotator()
    {
    }

    void SingleAxisTiltRotator::SetBaseScannerGeometry(const ScannerGeometry& baseScannerGeometry)
    {
        this->baseScannerGeometry = baseScannerGeometry;
    }

    const ScannerGeometry& SingleAxisTiltRotator::GetBaseScannerGeometry() const
    {
        return baseScannerGeometry;
    }

    ScannerGeometry& SingleAxisTiltRotator::GetBaseScannerGeometry()
    {
        return baseScannerGeometry;
    }

    ScannerGeometry SingleAxisTiltRotator::GetRotatedScannerGeometry(float tiltAngleInDegree, float xAxisTiltAngleInDegree)
    {
        ScannerGeometry geo(baseScannerGeometry.getProjectionType(), baseScannerGeometry.getResolution());

        float tiltAngleInRadians = -tiltAngleInDegree / 180.f * (float)M_PI;
        float xAxisTiltAngleInRadians = xAxisTiltAngleInDegree / 180.f * (float)M_PI;

        Vec3f detector = baseScannerGeometry.getDetectorBase();
        Vec3f source = baseScannerGeometry.getSourceBase();
        Vec3f horizontalPitch = baseScannerGeometry.getHorizontalPitch();
        Vec3f verticalPitch = baseScannerGeometry.getVerticalPitch();
        Rotate(detector, tiltAngleInRadians, xAxisTiltAngleInRadians);
        Rotate(source, tiltAngleInRadians, xAxisTiltAngleInRadians);
        Rotate(horizontalPitch, tiltAngleInRadians, xAxisTiltAngleInRadians);
        Rotate(verticalPitch, tiltAngleInRadians, xAxisTiltAngleInRadians);

        geo.set(source, detector, horizontalPitch, verticalPitch);
        return geo;
    }

    void SingleAxisTiltRotator::Rotate(Vec3f &coord, float tiltAngleInRadians, float xAxisTiltAngleInRadians)
    {
        if(xAxisTiltAngleInRadians == 0.0f)
        {
            coord = Vec3f(coord.x * cos(tiltAngleInRadians) - coord.z * sin(tiltAngleInRadians), coord.y, coord.x * sin(tiltAngleInRadians) + coord.z * cos(tiltAngleInRadians));
        }
        else
        {
            coord = Vec3f(coord.x, coord.y * cos(xAxisTiltAngleInRadians) + coord.z * sin(xAxisTiltAngleInRadians), -coord.y * sin(xAxisTiltAngleInRadians) + coord.z * cos(xAxisTiltAngleInRadians)); //rotate the point around x axis
            Vec3f axis = Normalize(Vec3f(0.f, cos(xAxisTiltAngleInRadians), -sin(xAxisTiltAngleInRadians))); //y axis around x
            Vec3f tempPoint = coord;

            coord.x = (cos(tiltAngleInRadians) + (1 - cos(tiltAngleInRadians))*axis.x*axis.x)        * tempPoint.x + ((1 - cos(tiltAngleInRadians))*axis.x*axis.y - sin(tiltAngleInRadians)*axis.z) * tempPoint.y + ((1 - cos(tiltAngleInRadians))*axis.x*axis.z + sin(tiltAngleInRadians)*axis.y) * tempPoint.z;
            coord.y = ((1 - cos(tiltAngleInRadians))*axis.x*axis.y + sin(tiltAngleInRadians)*axis.z) * tempPoint.x + (cos(tiltAngleInRadians) + (1 - cos(tiltAngleInRadians))*axis.y*axis.y)        * tempPoint.y + ((1 - cos(tiltAngleInRadians))*axis.y*axis.z - sin(tiltAngleInRadians)*axis.x) * tempPoint.z;
            coord.z = ((1 - cos(tiltAngleInRadians))*axis.x*axis.z - sin(tiltAngleInRadians)*axis.y) * tempPoint.x + ((1 - cos(tiltAngleInRadians))*axis.y*axis.z + sin(tiltAngleInRadians)*axis.x) * tempPoint.y + (cos(tiltAngleInRadians) + (1 - cos(tiltAngleInRadians))*axis.z*axis.z)        * tempPoint.z;
        }
    }
}