#pragma once

#include "model/geometry/ScannerGeometry.h"
#include "model/volume/Volume.h"

namespace ettention
{
    class VariableContainer;

    class GeometricSetup
    {
    private:
        ScannerGeometry* scannerGeometry;
        bool wasScannerGeometrySet;
        Volume* volume;
        unsigned int currentSubVolumeIndex;
        VariableContainer* projectionVariableContainer;
        VariableContainer* volumeVariableContainer;
        Vec2ui resolution;

    public:
        GeometricSetup(const Vec2ui& resolution, Volume* volume = 0);
        GeometricSetup(ScannerGeometry* scannerGeometry, Volume* volume = 0);
        virtual ~GeometricSetup();
  
        void setCurrentSubVolumeIndex(unsigned int subVolumeIndex);
        unsigned int getCurrentSubVolumeIndex() const;
        void setVolume(Volume* volume);
        Volume* getVolume() const;

        void setScannerGeometry(ScannerGeometry* scannerGeometry);
        ScannerGeometry* getScannerGeometry() const;
        const Vec2ui& getResolution() const;

        VariableContainer* getProjectionVariableContainer() const;
        VariableContainer* getVolumeVariableContainer() const;
    };
}
