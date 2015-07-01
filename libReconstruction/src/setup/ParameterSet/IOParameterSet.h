#pragma once
#include "setup/ParameterSet/OutputFormatParameterSet.h"
#include "setup/ParameterSet/ParameterSet.h"

namespace ettention
{
    class IOParameterSet : public ParameterSet
    {
    public:

        enum FileMode
        {
            UNSPECIFIED = -1,
            UNSIGNED_8 = 0,
            SIGNED_16 = 1,
            FLOAT_32 = 2,
            UNSIGNED_16 = 6
        };

        enum VolumeRotation
        {
            ALONG_XY = 0,
            ALONG_XZ = 1
        };

        enum PhysicalDistanceUnit
        {
            MILLIMETER = 0,
            MICROMETER = 1
        };

        IOParameterSet(const ParameterSource* parameterSource);
        IOParameterSet(std::string inputStackFileName);
        IOParameterSet(std::string inputStackFileName, std::string outputVolumeFileName);
        ~IOParameterSet();

        boost::filesystem::path OutputVolumeFileName() const;
        boost::filesystem::path InputStackFileName() const;
        FileMode Mode() const;
        boost::filesystem::path TiltAnglesFileName() const;
        bool LogaritmizeData() const;
        VolumeRotation OutputVolumeRotation() const;
        std::string OutputVolumeFormat() const;
        float XAxisTilt() const;

        static FileMode parseFileMode(const std::string& modeString);
        static VolumeRotation parseVolumeRotation(const std::string& rotationString);

    protected:
        boost::filesystem::path outputVolumeFileName;
        boost::filesystem::path inputStackFileName;
        boost::filesystem::path tiltAnglesFileName;
        FileMode mode;
        bool logaritmizeData;
        VolumeRotation outputVolumeRotation;
        std::string outputVolumeFormat;
        float xAxisTilt;
    };

    std::ostream& operator<<(std::ostream& oss, IOParameterSet::FileMode mode);
}