#include "stdafx.h"
#include "SingleAxisTiltRotator.h"

namespace ettention
{
    SingleAxisTiltRotator::SingleAxisTiltRotator()
        : baseScannerGeometry(nullptr)
    {
    }

    SingleAxisTiltRotator::~SingleAxisTiltRotator()
    {
        delete baseScannerGeometry;
    }

    void SingleAxisTiltRotator::setBaseScannerGeometry(ScannerGeometry* baseScannerGeometry)
    {
        if( this->baseScannerGeometry )
            delete this->baseScannerGeometry;

        this->baseScannerGeometry = baseScannerGeometry;
    }

    ScannerGeometry* SingleAxisTiltRotator::getBaseScannerGeometry() const
    {
        return baseScannerGeometry;
    }

    ScannerGeometry* SingleAxisTiltRotator::getBaseScannerGeometry()
    {
        return baseScannerGeometry;
    }

    ScannerGeometry* SingleAxisTiltRotator::getRotatedScannerGeometry(float tiltAngleInDegree, float xAxisTiltAngleInDegree) 
    {
        ScannerGeometry* geometry = baseScannerGeometry->clone();

        float tiltAngleInRadians = -tiltAngleInDegree / 180.f * (float)M_PI;
        float xAxisTiltAngleInRadians = xAxisTiltAngleInDegree / 180.f * (float)M_PI;

        Vec3f detector = baseScannerGeometry->getDetectorBase();
        Vec3f source = baseScannerGeometry->getSourceBase();
        Vec3f horizontalPitch = baseScannerGeometry->getHorizontalPitch();
        Vec3f verticalPitch = baseScannerGeometry->getVerticalPitch();
        doRotate(detector, tiltAngleInRadians, xAxisTiltAngleInRadians);
        doRotate(source, tiltAngleInRadians, xAxisTiltAngleInRadians);
        doRotate(horizontalPitch, tiltAngleInRadians, xAxisTiltAngleInRadians);
        doRotate(verticalPitch, tiltAngleInRadians, xAxisTiltAngleInRadians);

        geometry->set(source, detector, horizontalPitch, verticalPitch);
        return geometry;
    }

    void SingleAxisTiltRotator::doRotate(Vec3f &coord, float tiltAngleInRadians, float xAxisTiltAngleInRadians)
    {
        if(xAxisTiltAngleInRadians == 0.0f)
        {
            coord = Vec3f(coord.x * cos(tiltAngleInRadians) - coord.z * sin(tiltAngleInRadians), coord.y, coord.x * sin(tiltAngleInRadians) + coord.z * cos(tiltAngleInRadians));
        }
        else
        {
            coord = Vec3f(coord.x, coord.y * cos(xAxisTiltAngleInRadians) + coord.z * sin(xAxisTiltAngleInRadians), -coord.y * sin(xAxisTiltAngleInRadians) + coord.z * cos(xAxisTiltAngleInRadians)); //rotate the point around x axis
            Vec3f axis = doNormalize(Vec3f(0.f, cos(xAxisTiltAngleInRadians), -sin(xAxisTiltAngleInRadians))); //y axis around x
            Vec3f tempPoint = coord;

            coord.x = (cos(tiltAngleInRadians) + (1 - cos(tiltAngleInRadians))*axis.x*axis.x)        * tempPoint.x + ((1 - cos(tiltAngleInRadians))*axis.x*axis.y - sin(tiltAngleInRadians)*axis.z) * tempPoint.y + ((1 - cos(tiltAngleInRadians))*axis.x*axis.z + sin(tiltAngleInRadians)*axis.y) * tempPoint.z;
            coord.y = ((1 - cos(tiltAngleInRadians))*axis.x*axis.y + sin(tiltAngleInRadians)*axis.z) * tempPoint.x + (cos(tiltAngleInRadians) + (1 - cos(tiltAngleInRadians))*axis.y*axis.y)        * tempPoint.y + ((1 - cos(tiltAngleInRadians))*axis.y*axis.z - sin(tiltAngleInRadians)*axis.x) * tempPoint.z;
            coord.z = ((1 - cos(tiltAngleInRadians))*axis.x*axis.z - sin(tiltAngleInRadians)*axis.y) * tempPoint.x + ((1 - cos(tiltAngleInRadians))*axis.y*axis.z + sin(tiltAngleInRadians)*axis.x) * tempPoint.y + (cos(tiltAngleInRadians) + (1 - cos(tiltAngleInRadians))*axis.z*axis.z)        * tempPoint.z;
        }
    }
}