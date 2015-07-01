#include "stdafx.h"
#include "GeometricSetup.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/variables/VariableContainer.h"

namespace ettention
{
    GeometricSetup::GeometricSetup(ScannerGeometry::ProjectionType projectionType, const Vec2ui& resolution, Volume* volume)
        : scannerGeometry(projectionType, resolution)
        , volume(volume)
        , currentSubVolumeIndex(0)
        , projectionVariableContainer(new VariableContainer)
        , volumeVariableContainer(new VariableContainer)
    {
    }

    GeometricSetup::GeometricSetup(const ScannerGeometry& intialScannerGeometry, Volume* volume)
        : scannerGeometry(intialScannerGeometry)
        , volume(volume)
        , currentSubVolumeIndex(0)
        , projectionVariableContainer(new VariableContainer)
        , volumeVariableContainer(new VariableContainer)
    {
    }

    GeometricSetup::~GeometricSetup()
    {
        delete volumeVariableContainer;
        delete projectionVariableContainer;
    }

    void GeometricSetup::setCurrentSubVolumeIndex(unsigned int subVolumeIndex)
    {
        this->currentSubVolumeIndex = subVolumeIndex;
    }

    unsigned int GeometricSetup::getCurrentSubVolumeIndex() const
    {
        return this->currentSubVolumeIndex;
    }

    void GeometricSetup::setScannerGeometry(const ScannerGeometry& scannerGeometry)
    {
        this->scannerGeometry = scannerGeometry;
    }

    const ScannerGeometry& GeometricSetup::getScannerGeometry() const
    {
        return scannerGeometry;
    }

    const Vec2ui& GeometricSetup::getResolution() const
    {
        return scannerGeometry.getResolution();
    }

    Volume* GeometricSetup::getVolume() const
    {
        return this->volume;
    }

    void GeometricSetup::setVolume(Volume* volume)
    {
        this->volume = volume;
    }

    ScannerGeometry::ProjectionType GeometricSetup::getProjectionType() const
    {
        return scannerGeometry.getProjectionType();
    }

    VariableContainer* GeometricSetup::getProjectionVariableContainer() const
    {
        projectionVariableContainer->Clear();
        projectionVariableContainer->Push(KernelVariable::CreateFromVec3f("ray", scannerGeometry.getRayDirection()));
        projectionVariableContainer->Push(KernelVariable::CreateFromVec3f("invRay", scannerGeometry.getRayDirection().Inverse()));
        projectionVariableContainer->Push(KernelVariable::CreateFromVec3f("source", scannerGeometry.getSourceBase()));
        projectionVariableContainer->Push(KernelVariable::CreateFromVec3f("detector", scannerGeometry.getDetectorBase()));
        projectionVariableContainer->Push(KernelVariable::CreateFromVec3f("horizontalPitch", scannerGeometry.getHorizontalPitch()));
        projectionVariableContainer->Push(KernelVariable::CreateFromVec3f("verticalPitch", scannerGeometry.getVerticalPitch()));
        float rayVoxelIntersectionLength = NumericalAlgorithms::GetIntersectionLengthOfRayThroughVoxelCenter(scannerGeometry.getRayDirection(), getVolume()->Properties().GetVoxelSize());
        projectionVariableContainer->Push(KernelVariable::CreateFromFloat("rayVoxelIntersectionLength", rayVoxelIntersectionLength));
        return projectionVariableContainer;
    }

    VariableContainer* GeometricSetup::getVolumeVariableContainer() const
    {
        volumeVariableContainer->Clear();
        volumeVariableContainer->Push(KernelVariable::CreateFromVec3ui("subVolumeResolution", getVolume()->Properties().GetSubVolumeResolution(currentSubVolumeIndex)));
        volumeVariableContainer->Push(KernelVariable::CreateFromVec3ui("volumeResolution", getVolume()->Properties().GetVolumeResolution()));
        volumeVariableContainer->Push(KernelVariable::CreateFromVec3f("bBoxMin", getVolume()->Properties().GetSubVolumeBoundingBox(currentSubVolumeIndex).GetMin()));
        volumeVariableContainer->Push(KernelVariable::CreateFromVec3f("bBoxMax", getVolume()->Properties().GetSubVolumeBoundingBox(currentSubVolumeIndex).GetMax()));
        volumeVariableContainer->Push(KernelVariable::CreateFromVec3f("bBoxMinComplete", getVolume()->Properties().GetVolumeBoundingBox().GetMin()));
        volumeVariableContainer->Push(KernelVariable::CreateFromVec3f("bBoxMaxComplete", getVolume()->Properties().GetVolumeBoundingBox().GetMax()));
        volumeVariableContainer->Push(KernelVariable::CreateFromVec3f("voxelSize", getVolume()->Properties().GetVoxelSize()));
        return volumeVariableContainer;
    }
}