#include "stdafx.h"
#include "GeometricSetup.h"
#include "framework/NumericalAlgorithms.h"
#include "gpu/variables/VariableContainer.h"

namespace ettention
{
    GeometricSetup::GeometricSetup(const Vec2ui& resolution, Volume* volume)
        : scannerGeometry(nullptr)
        , volume(volume)
        , currentSubVolumeIndex(0)
        , projectionVariableContainer(new VariableContainer)
        , volumeVariableContainer(new VariableContainer)
        , resolution(resolution)
    {
    }

    GeometricSetup::GeometricSetup(ScannerGeometry* initialScannerGeometry, Volume* volume)
        : scannerGeometry(initialScannerGeometry)
        , volume(volume)
        , currentSubVolumeIndex(0)
        , projectionVariableContainer(new VariableContainer)
        , volumeVariableContainer(new VariableContainer)
        , resolution(initialScannerGeometry->getResolution())
    {
    }

    GeometricSetup::~GeometricSetup()
    {
		delete scannerGeometry;
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

    void GeometricSetup::setScannerGeometry(ScannerGeometry* scannerGeometry)
    {
        if( this->scannerGeometry == scannerGeometry )
            return;

        if( this->scannerGeometry )
            delete(this->scannerGeometry);
        this->scannerGeometry = scannerGeometry;
    }

    ScannerGeometry* GeometricSetup::getScannerGeometry() const
    {
        if( !scannerGeometry )
            throw Exception("GeometricSetup trying to getScannerGeometry before initializing scannerGeometry");

        return scannerGeometry;
    }

    const Vec2ui& GeometricSetup::getResolution() const
    {
        return resolution;
    }

    Volume* GeometricSetup::getVolume() const
    {
        if(volume)
            return this->volume;

        throw Exception("GeometricSetup trying to getVolume before setting any");
    }

	void GeometricSetup::setVolume(Volume* volume)
    {
        this->volume = volume;
    }

    VariableContainer* GeometricSetup::getProjectionVariableContainer() const
    {
        if( !scannerGeometry )
            throw Exception("GeometricSetup trying to getProjectionVariableContainer before initializing scannerGeometry");

        projectionVariableContainer->clear();
		scannerGeometry->addToVariableContainer( projectionVariableContainer );
		float rayVoxelIntersectionLength = NumericalAlgorithms::getIntersectionLengthOfRayThroughVoxelCenter(scannerGeometry->getRayDirection(), getVolume()->getProperties().getVoxelSize());
		projectionVariableContainer->push(KernelVariable::createFromFloat("rayVoxelIntersectionLength", rayVoxelIntersectionLength));
        Vec3f getVoxelType = getVolume()->getProperties().getVoxelSize();
        return projectionVariableContainer;
    }

    VariableContainer* GeometricSetup::getVolumeVariableContainer() const
    {
        volumeVariableContainer->clear();
        volumeVariableContainer->push(KernelVariable::createFromVec3ui("subVolumeResolution", getVolume()->getProperties().getSubVolumeResolution(currentSubVolumeIndex)));
        volumeVariableContainer->push(KernelVariable::createFromVec3ui("volumeResolution", getVolume()->getProperties().getVolumeResolution()));
        volumeVariableContainer->push(KernelVariable::createFromVec3f("bBoxMin", getVolume()->getProperties().getSubVolumeBoundingBox(currentSubVolumeIndex).getMin()));
        volumeVariableContainer->push(KernelVariable::createFromVec3f("bBoxMax", getVolume()->getProperties().getSubVolumeBoundingBox(currentSubVolumeIndex).getMax()));
        volumeVariableContainer->push(KernelVariable::createFromVec3f("bBoxMinComplete", getVolume()->getProperties().getVolumeBoundingBox().getMin()));
        volumeVariableContainer->push(KernelVariable::createFromVec3f("bBoxMaxComplete", getVolume()->getProperties().getVolumeBoundingBox().getMax()));
        volumeVariableContainer->push(KernelVariable::createFromVec3f("voxelSize", getVolume()->getProperties().getVoxelSize()));
        return volumeVariableContainer;
    }
}