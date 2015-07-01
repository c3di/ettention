#pragma once
#include "model/geometry/ScannerGeometry.h"
#include "model/volume/Volume.h"

namespace ettention
{
    class VariableContainer;

    class GeometricSetup
    {
    private:
        ScannerGeometry scannerGeometry;
        Volume* volume;
        unsigned int currentSubVolumeIndex;
        VariableContainer* projectionVariableContainer;
        VariableContainer* volumeVariableContainer;

    public:
        GeometricSetup(ScannerGeometry::ProjectionType projectionType, const Vec2ui& resolution, Volume* volume = 0);
        GeometricSetup(const ScannerGeometry& initialScannerGeometry, Volume* volume = 0);
        virtual ~GeometricSetup();
  
        void setCurrentSubVolumeIndex(unsigned int subVolumeIndex);
        unsigned int getCurrentSubVolumeIndex() const;
        void setVolume(Volume* volume);
        Volume* getVolume() const;

        void setScannerGeometry(const ScannerGeometry& scannerGeometry);
        const ScannerGeometry& getScannerGeometry() const;
        const Vec2ui& getResolution() const;

        ScannerGeometry::ProjectionType getProjectionType() const;

        VariableContainer* getProjectionVariableContainer() const;
        VariableContainer* getVolumeVariableContainer() const;
    };
}
