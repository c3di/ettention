#include "stdafx.h"
#include "ScannerGeometry.h"
#include "framework/geometry/Visualizer.h"
#include "framework/NumericalAlgorithms.h"
#include "io/datasource/ImageStackDatasource.h"
#include "gpu/variables/KernelVariable.h"
#include "gpu/variables/VariableContainer.h"

namespace ettention
{
    ScannerGeometry::ScannerGeometry()
        : sourceBase(0.0f, 0.0f, 0.0f)
        , detectorBase(0.0f, 0.0f, 0.0f)
        , horizontalPitch(0.0f, 0.0f, 0.0f)
        , verticalPitch(0.0f, 0.0f, 0.0f)
        , resolution(0, 0)
    {
    }

    ScannerGeometry::ScannerGeometry(const ScannerGeometry* other)
        : sourceBase(other->sourceBase)
        , detectorBase(other->detectorBase)
        , horizontalPitch(other->horizontalPitch)
        , verticalPitch(other->verticalPitch)
        , resolution(other->resolution)
    {
    }

    ScannerGeometry::ScannerGeometry(ImageStackDatasource* inputStack)
        : resolution(inputStack->getResolution())
    {
    }

    ScannerGeometry::ScannerGeometry(const Vec2ui& resolution)
        : resolution(resolution)
    {
    }

    ScannerGeometry::ScannerGeometry(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch, const Vec2ui& resolution)
        : sourceBase(sourceBase)
        , detectorBase(detectorBase)
        , horizontalPitch(horizontalPitch)
        , verticalPitch(verticalPitch)
        , resolution(resolution)
    {
    }

    ScannerGeometry::~ScannerGeometry()
    {
    }

    void ScannerGeometry::set(const Vec3f& sourceBase, const Vec3f& detectorBase, const Vec3f& horizontalPitch, const Vec3f& verticalPitch)
    {
        this->sourceBase = sourceBase;
        this->detectorBase = detectorBase;
        this->horizontalPitch = horizontalPitch;
        this->verticalPitch = verticalPitch;
    }

    void ScannerGeometry::setResolution(const Vec2ui& resolution)
    {
        this->resolution = resolution;
    }

    const Vec2ui& ScannerGeometry::getResolution() const
    {
        return resolution;
    }

    const Vec3f& ScannerGeometry::getSourceBase() const
    {
        return sourceBase;
    }

    const Vec3f& ScannerGeometry::getDetectorBase() const
    {
        return detectorBase;
    }

    Vec3f ScannerGeometry::getDetectorCenter() const
    {
        return detectorBase + 0.5f * ((float)resolution.x * horizontalPitch + (float)resolution.y * verticalPitch);
    }

    const Vec3f& ScannerGeometry::getHorizontalPitch() const
    {
        return horizontalPitch;
    }

    const Vec3f& ScannerGeometry::getVerticalPitch() const
    {
        return verticalPitch;
    }

    void ScannerGeometry::addToVariableContainer(VariableContainer* container) const
    {
        container->push(KernelVariable::createFromVec3f("ray", getRayDirection()));
        container->push(KernelVariable::createFromVec3f("invRay", getRayDirection().makeInverse()));
        container->push(KernelVariable::createFromVec3f("source", getSourceBase()));
        container->push(KernelVariable::createFromVec3f("detector", getDetectorBase()));
        container->push(KernelVariable::createFromVec3f("horizontalPitch", getHorizontalPitch()));
        container->push(KernelVariable::createFromVec3f("verticalPitch", getVerticalPitch()));
    }

}