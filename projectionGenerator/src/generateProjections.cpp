#include "stdafx.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "setup/parametersource/ManualParameterSource.h"
#include "setup/ParameterSet/HardwareParameterSet.h"
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLMemBuffer.h"
#include "model/volume/Volume.h"
#include "model/volume/GPUMappedVolume.h"
#include "io/deserializer/VolumeDeserializer.h"
#include "io/serializer/ImageSerializer.h"
#include "io/datasource/ImageStackDirectoryDatasource.h"
#include "io/TF_Datasource.h"
#include "framework/Framework.h"
#include "framework/VectorAlgorithms.h"
#include "reconstruction/projections/STEMForwardProjection.h"
#include "GenerateProjectionsParameterSource.h"
#include "projection_pattern/FocalStackProjectionPattern.h"
#include "projection_pattern/SpiralProjectionPattern.h"

#ifdef WIN32
#include <conio.h>
#else
#include "conio_linux.h"
#endif

const float detectorDistance = 1024.0f;

using namespace ettention;
using namespace ettention::stem;

ScannerGeometry GetScannerGeometry(Volume* volume, unsigned int projectionResolution)
{
    Vec3f sourcePosition(volume->Properties().GetVolumeResolution().x * -0.5f, volume->Properties().GetVolumeResolution().y * -0.5f, -1.0f * detectorDistance);
    Vec3f detectorPosition(volume->Properties().GetVolumeResolution().x * -0.5f, volume->Properties().GetVolumeResolution().y * -0.5f, 1.0f * detectorDistance);
    const float scaleFactor = volume->Properties().GetVolumeResolution().x / ((float)projectionResolution);
    Vec3f horizontalPitch(scaleFactor, 0.0f, 0.0f);
    Vec3f verticalPitch(0.0f, scaleFactor, 0.0f);
    ScannerGeometry geo(ScannerGeometry::PROJECTION_PARALLEL, Vec2ui(projectionResolution, projectionResolution));
    geo.set(sourcePosition, detectorPosition, horizontalPitch, verticalPitch);
    return geo;
}

int main(int argc, char* argv[])
{
    std::cout << "Generating projections for convergent beams electron microscopy." << std::endl;
    try
    {
        GenerateProjectionsParameterSource parameters(argc, argv);
        parameters.parse();

        if(parameters.parameterExists("help"))
        {
            std::cout << parameters.getDescription() << "\n";
        }

        std::string inputFileName = parameters.getStringParameter("inputFile");
        std::string outputDirectoryName = parameters.getStringParameter("outputDirectory");
        float openingAngle = parameters.getFloatParameter("halfAngle");
        unsigned int resolution = parameters.getUIntParameter("resolution");
        float tiltAngleDistance = parameters.getFloatParameter("tiltAngleDistance");
        unsigned int focalPositionsPerTilt = parameters.getUIntParameter("focalPositions");
        PatternType pattern = ProjectionPattern::parsePatternType(parameters.getStringParameter("pattern"));
        float minTiltAngle = parameters.getFloatParameter("minTiltAngle");
        float maxTiltAngle = parameters.getFloatParameter("maxTiltAngle");
        bool metaDataOnly = parameters.getBoolParameter("metaDataOnly");
        unsigned int samplesPerPixel = parameters.getUIntParameter("samples");

        Vec2ui projectionResolution = Vec2ui(resolution, resolution);

        auto volume = VolumeDeserializer::load(inputFileName, VoxelType::FLOAT_32);
        std::cout << "Volume resolution: " << volume->Properties().GetVolumeResolution() << std::endl;
        std::cout << "Projection resolution: " << resolution << std::endl;

        std::shared_ptr<ProjectionPattern> generator;
        if(pattern == PatternType::STACK)
            generator = std::shared_ptr<ProjectionPattern>(new FocalStackProjectionPattern(openingAngle, minTiltAngle, maxTiltAngle, tiltAngleDistance, focalPositionsPerTilt, 2.0f * (float)volume->Properties().GetVolumeResolution().z, detectorDistance));
        else if(pattern == PatternType::SPIRAL)
            generator = std::shared_ptr<ProjectionPattern>(new SpiralProjectionPattern(openingAngle, minTiltAngle, maxTiltAngle, tiltAngleDistance, focalPositionsPerTilt, 2.0f * (float)volume->Properties().GetVolumeResolution().z));
        else
            throw std::runtime_error("illegal pattern: " + pattern);
        auto projections = generator->getProjectionMetaData();

        Vec3ui volumeResolution = volume->Properties().GetVolumeResolution();
        VolumeParameterSet* volumeOptions = new VolumeParameterSet(volumeResolution);
        SingleAxisTiltRotator satRotator;
        satRotator.SetBaseScannerGeometry(GetScannerGeometry(volume, resolution));
        auto geometricSetup = new GeometricSetup(satRotator.GetBaseScannerGeometry());

        Framework* framework = new Framework(Logger::getInstance());

        CLMemBuffer* volumeBuffer = new CLMemBuffer(framework->getOpenCLStack(), volume->Properties().GetSubVolumeByteWidth(0));
        GPUMappedVolume* mappedVolume = new GPUMappedVolume(framework->getOpenCLStack(), volume);

        STEMForwardProjectionKernel* forwardKernel = new STEMForwardProjectionKernel(framework,
                                                                                     projectionResolution,
                                                                                     geometricSetup,
                                                                                     satRotator.GetBaseScannerGeometry().getSourceBase(),
                                                                                     mappedVolume,
                                                                                     samplesPerPixel);
        volumeBuffer->transferFrom(volume->getRawDataOfSubvolume(0));

        unsigned int projectionNumber = 0;

        std::cout << "[" << std::flush;
        std::vector<TFProjectionProperties> projectionProperties;
        std::vector<std::string> projectionFilenames;
        for(auto it = projections.begin(); it != projections.end(); ++it)
        {
            std::stringstream fname;

            fname << outputDirectoryName << "/projection_" << projectionNumber;
            projectionNumber++;

            if(!metaDataOnly)
            {
                forwardKernel->setProjectionProperties(*it);
                geometricSetup->setScannerGeometry(satRotator.GetRotatedScannerGeometry((*it).GetTiltAngle()));
                forwardKernel->getOutput()->getBufferOnGPU()->clear();
                forwardKernel->run();

                ImageSerializer::writeImage(fname.str(), forwardKernel->getOutput(), ImageSerializer::TIFF_GRAY_32);
            }

            boost::filesystem::path filepath = fname.str() + ".tif";
            std::string relativeNameToImageFile = boost::filesystem::basename(filepath) + boost::filesystem::extension(filepath);
            projectionProperties.push_back(*it);
            projectionFilenames.push_back(relativeNameToImageFile);

            std::cout << "=" << std::flush;
            if(_kbhit() && _getch() == 27)
            {
                std::cout << std::endl << "user canceled data generation" << std::endl;
                exit(2);
            }

        }
        std::cout << "]" << std::endl;

        TF_Datasource::writeMetaDataFile(outputDirectoryName + "/metadata.xml", projectionProperties, projectionFilenames, satRotator.GetBaseScannerGeometry(), geometricSetup->getVolume()->Properties());
        delete framework;
        delete forwardKernel;
        delete mappedVolume;
        delete volumeOptions;
    }
    catch(const std::exception& e)
    {
        std::cout << "Failed to generate projections: " << e.what() << std::endl;
    }
}