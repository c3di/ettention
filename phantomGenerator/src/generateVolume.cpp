#include "stdafx.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "model/volume/Volume.h"
#include "io/serializer/VolumeSerializer.h"
#include "io/serializer/MRCWriter.h"
#include "GenerateVolumeParameterSource.h"
#include "phantom/PlottableShape.h"
#include "phantom/SheppLoganShape.h"
#include "phantom/Diagonal3Shape.h"
#include "phantom/ClusteredParticlesShape.h"
#include "framework/error/Exception.h"

using namespace ettention;

Volume* allocateVolume(unsigned int resolution, float background_intensity) 
{
    Vec3ui volumeResolution(resolution, resolution, resolution);
    Volume* volume = new Volume(VolumeProperties(VoxelType::FLOAT_32, VolumeParameterSet(volumeResolution), 1), background_intensity);
    return volume;
}

void generatePhantomSheppLogan(Volume* volume, unsigned int nSamples, bool mask)
{
    ComposedShape* sheppLogan = new SheppLoganShape(volume, mask);
    sheppLogan->plotToVolume(nSamples);
    delete sheppLogan;
}

void generatePhantomParticles(Volume* volume, unsigned int nSamples)
{
    float particleSize = (float) volume->Properties().GetVolumeResolution().x / 128.0f;
    ClusteredParticlesShape* particles = new ClusteredParticlesShape(volume, 8, 64, particleSize, 16.0f);
    particles->plotToVolume(nSamples);
    delete particles;
}

void generatePhantomDiagnoal3(Volume* volume, unsigned int nSamples)
{
    float particleSize = (float) volume->Properties().GetVolumeResolution().x / 128.0f;
    Diagonal3Shape* particles = new Diagonal3Shape(volume);
    particles->plotToVolume(nSamples);
    delete particles;
}

int main(int argc, char* argv[])
{
    std::cout << "Generating synthetic volume for electron microscopy." << std::endl;
    try {
        GenerateVolumeParameterSource parameters(argc, argv);
        parameters.parse();

        if(parameters.parameterExists("help")) {
            std::cout << parameters.getDescription() << "\n";
        }

        std::string filename = parameters.getStringParameter("outputFile");
        IOParameterSet::FileMode mode = IOParameterSet::parseFileMode(parameters.getStringParameter("fileMode"));
        bool useIModFormat = parameters.parameterExists("useIModFormat");
        bool mask = parameters.parameterExists("mask");
        unsigned int resolution = parameters.getUIntParameter("resolution");
        unsigned int nSamples = parameters.getUIntParameter("samples");
        std::string phantomShape = parameters.getStringParameter("shape");
        float background_intensity= parameters.getFloatParameter("background");

        std::cout << "Generating volume with shape " << phantomShape << std::endl;
        std::cout << "  resolution                 " << resolution << " x " << resolution << " x " << resolution << std::endl;
        std::cout << "  samples                    " << nSamples << " x " << nSamples << " x " << nSamples << std::endl;

        Volume* volume = allocateVolume(resolution, background_intensity);

        if(phantomShape.compare("diagonal_3") == 0)
            generatePhantomDiagnoal3(volume, nSamples);
        else if(phantomShape.compare("shepp_logan") == 0)
            generatePhantomSheppLogan(volume, nSamples, mask);
        else if(phantomShape.compare("particles") == 0)
            generatePhantomParticles(volume, nSamples);
        else
            throw Exception("illegal shape " + phantomShape + " valid values are diagonal_3, shepp_logan and particles");

        MRCWriter writer;
        const OutputFormatParameterSet format;
        writer.write(volume, filename, &format);
        delete volume;
    } catch (const std::exception& e) {
        std::cout << "Failed to generate volume: "  << e.what() << std::endl;
        return 1;
    }
    return 0;
}
