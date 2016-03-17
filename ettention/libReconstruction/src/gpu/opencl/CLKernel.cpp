#include "stdafx.h"
#include "CLKernel.h"
#include "CLMemBuffer.h"
#include "CLMemImage.h"
#include "CLMemoryStructure.h"
#include "CLSampler.h"
#include "framework/Framework.h"
#include "setup/parameterset/DebugParameterSet.h"
#include "setup/parameterset/OptimizationParameterSet.h"

namespace ettention
{
    CLKernel::CLKernel(Framework* framwork, const std::string& caption, const std::string& sourceCode, const std::string& kernelName, const std::string& additionalKernelDefines)
        : clal(framwork->getOpenCLStack())
        , caption(caption)
        , debugKernelParameters(framwork->getParameterSet()->get<DebugParameterSet>()->shouldDisplayKernelParameters())
        , localWorkSize(0)
    {
        getParameterList(sourceCode, kernelName, kernelParameters);
        std::string tempSourceCode = sourceCode;
        fixSourceForImageSupport(tempSourceCode);
        program = generateProgramFromSourceCode(tempSourceCode, kernelName, additionalKernelDefines + getKernelDefines(framwork));
        clKernel = clCreateKernel(program, kernelName.c_str(), &err);
        CL_ASSERT(err);
        globalWorkSize[0] = globalWorkSize[1] = globalWorkSize[2] = 1;
    }

    CLKernel::~CLKernel()
    {
        delete[] localWorkSize;
        while(!parameterBuffers.empty())
        {
            delete parameterBuffers.begin()->second;
            parameterBuffers.erase(parameterBuffers.begin());
        }
        CL_ASSERT(clReleaseKernel(clKernel));
        CL_ASSERT(clReleaseProgram(program));
    }

    void CLKernel::setArgument(unsigned int paramIndex, const void* rawData, size_t rawSize)
    {
        CL_ASSERT(clSetKernelArg(clKernel, paramIndex, rawSize, rawData));
    }

    void CLKernel::setArgument(unsigned int paramIndex, CLMemBuffer* value)
    {
        setArgument(paramIndex, &value->getCLMem(), sizeof(cl_mem));
    }

    void CLKernel::setArgument(unsigned int paramIndex, CLMemObject* value)
    {
        setArgument(paramIndex, &value->getCLMem(), sizeof(cl_mem));
    }

    void CLKernel::setArgument(unsigned int paramIndex, CLMemImage* value)
    {
        setArgument(paramIndex, &value->getCLMem(), sizeof(cl_mem));
    }

    void CLKernel::setArgument(unsigned int paramIndex, CLSampler* value)
    {
        setArgument(paramIndex, &value->getCLSampler(), sizeof(cl_mem));
    }

    void CLKernel::setArgument(unsigned int paramIndex, GPUMappedVolume* volume)
    {
        volume->ensureIsUpToDateOnGPU();
        if( volume->getMappedSubVolume()->doesUseImage() )
        {
            volume->getMappedSubVolume()->ensureImageIsUpToDate();
            setArgument(paramIndex, volume->getMappedSubVolume()->getImageOnGPU());
        } else {
            setArgument(paramIndex, volume->getMappedSubVolume()->getBufferOnGPU());
        }
    }

    void CLKernel::allocateArgument(const std::string& paramName, size_t paramSize)
    {
        missingArguments.erase(paramName);
        CL_ASSERT(clSetKernelArg(clKernel, getParameterNumber(paramName), paramSize, NULL));
    }

    void CLKernel::setArgument(unsigned int paramIndex, VariableContainer* value)
    {
        CLMemBuffer* buffer = 0;
        auto findIt = parameterBuffers.find(paramIndex);
        if(findIt != parameterBuffers.end())
        {
            if(value->getByteWidth() != findIt->second->getByteWidth())
            {
                delete findIt->second;
                findIt->second = new CLMemBuffer(clal, value->getByteWidth());
            }
            buffer = findIt->second;
        }
        else
        {
            parameterBuffers[paramIndex] = buffer = new CLMemBuffer(clal, value->getByteWidth());
        }

        std::size_t offset = 0;
        for(unsigned int i = 0; i < value->getVariableCount(); ++i)
        {
            buffer->transferFrom(value->get(i)->getData(), offset, value->get(i)->getDataSize());
            offset += value->get(i)->getDataSize();
        }
        setArgument(paramIndex, buffer);
        outputVariablesIfRequested(value);
    }

    void CLKernel::setArgument(unsigned int paramIndex, CLMemoryStructure* value)
    {
        value->setAsArgument(this, paramIndex);
    }

    void CLKernel::setGlobalWorkSize(unsigned int x, unsigned int y, unsigned int z)
    {
        globalWorkSize[0] = x;
        globalWorkSize[1] = y;
        globalWorkSize[2] = z;
    }

    void CLKernel::setGlobalWorkSize(const Vec2ui& xy)
    {
        setGlobalWorkSize(xy.x, xy.y);
    }

    void CLKernel::setGlobalWorkSize(const Vec3ui& xyz)
    {
        setGlobalWorkSize(xyz.x, xyz.y, xyz.z);
    }

    void CLKernel::setLocalWorkSize(unsigned int x, unsigned int y, unsigned int z)
    {
        if(!localWorkSize)
        {
            localWorkSize = new std::size_t[3];
        }
        localWorkSize[0] = x;
        localWorkSize[1] = y;
        localWorkSize[2] = z;
    }

    void CLKernel::setLocalWorkSize(const Vec2ui& xy)
    {
        setLocalWorkSize(xy.x, xy.y);
    }

    void CLKernel::setLocalWorkSize(const Vec3ui& xyz)
    {
        setLocalWorkSize(xyz.x, xyz.y, xyz.z);
    }

    void CLKernel::setAutomaticLocalWorkSize()
    {
        delete[] localWorkSize;
        localWorkSize = 0;
    }

    void CLKernel::run()
    {
        if(!missingArguments.empty())
        {
            std::string missingArgsString;
            std::string seperator = "";
            for(auto it = missingArguments.begin(); it != missingArguments.end(); ++it)
            {
                missingArgsString += seperator + *it;
                seperator = ", ";
            }
            throw GPUException("No arguments for kernel parameters given: " + missingArgsString + "!");
        }

        const unsigned int dimension = globalWorkSize[2] != 1 ? 3U : globalWorkSize[1] != 1 ? 2U : 1U;
        cl_event launchEvent;
        CL_ASSERT(clEnqueueNDRangeKernel(clal->getCommandQueue(), clKernel, dimension, NULL, globalWorkSize, localWorkSize, 0, NULL, &launchEvent));
        CL_ASSERT(clWaitForEvents(1, &launchEvent));
        clal->profileAndReleaseEvent("Kernel execution: " + caption, launchEvent, true);
        kernelParameters.getParameterNames(missingArguments);
    }

    cl_kernel CLKernel::getKernel()
    {
        return clKernel;
    }

    void CLKernel::skipOptionalArgument(const std::string& paramName)
    {
        missingArguments.erase(paramName);
    }

    void CLKernel::fixSourceForImageSupport(std::string& sourceCode)
    {
        if(clal->hasImageSupport())
        {
            sourceCode.insert(0, "#define IMAGE_SUPPORT \n");
        }
    }

    std::string CLKernel::getSource(const std::string& fileName)
    {
        std::string source;

        std::ifstream ifs(fileName, std::ifstream::in);
        if(!ifs.good())
        {
            std::string message("Unable to open OpenCL source file: " + fileName);
            throw std::runtime_error(message.c_str());
        }

        std::string source2(std::istreambuf_iterator<char>(ifs), (std::istreambuf_iterator<char>()));
        source.assign(source2);

        fixSourceForImageSupport(source);

        return source;
    }

    cl_program CLKernel::generateProgramFromSourceCode(const std::string& sourceCode, const std::string& kernelName, const std::string& additionalDefines)
    {
        cl_int err = CL_SUCCESS;
        const char* sourceCodeLines = sourceCode.c_str();
        cl_program program = clCreateProgramWithSource(clal->getContext(), 1, &sourceCodeLines, NULL, &err);
        CL_ASSERT(err);
        cl_device_id dev = clal->getDeviceId();
        err = clBuildProgram(program, 1, &dev, additionalDefines.c_str(), NULL, NULL);

        size_t buildLogSize = 0;
        CL_ASSERT(clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &buildLogSize));
        if(buildLogSize > 2)
        {
            char* buildLog = new char[buildLogSize];
            CL_ASSERT(clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, NULL));
            std::stringstream buildLogStream;
            buildLogStream << "** Build log ***************************************************" << std::endl;
            buildLogStream << buildLog << std::endl;
//             buildLogStream << "** Kernel source ***********************************************" << std::endl;
//             buildLogStream << sourceCode << std::endl;
            buildLogStream << "** Build log end ***********************************************" << std::endl;
            if(err == CL_SUCCESS)
            {
                LOGGER("Kernel build info for kernel " << kernelName << std::endl << buildLogStream.str());
            }
            else
            {
                LOGGER_ERROR("Compilation of kernel \"" << kernelName << "\" failed!" << std::endl << buildLogStream.str());
                std::cout << buildLogStream.str();
            }
            delete[] buildLog;
        }
        else
        {
            CL_ASSERT(err);
        }
        return program;
    }

    void CLKernel::getParameterList(const std::string& sourceCode, const std::string& kernelName, CLKernelParameterParser::KernelParameters& kernelParameters)
    {
        CLKernelParameterParser extractor(sourceCode, kernelName);
        extractor.storeParametersTo(kernelParameters);
        kernelParameters.getParameterNames(missingArguments);
    }

    int CLKernel::getParameterNumber(const std::string& parameterName)
    {
        return kernelParameters.getParameterByName(parameterName).number;
    }

    void CLKernel::outputVariablesIfRequested(const VariableContainer* container)
    {
        if(debugKernelParameters)
        {
            for(unsigned int i = 0; i < container->getVariableCount(); ++i)
            {
                container->get(i)->writeToStream(std::cout);
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }

    std::string CLKernel::getKernelDefines(Framework* framework)
    {
        auto parameterSet = framework->getParameterSet()->get<OptimizationParameterSet>();
        return Voxel::getKernelDefine(parameterSet->getVoxelRepresentation());
    }

    void CLKernel::queryAndApplyOptimumWorkSize1D(unsigned int minimumGlobalSize, unsigned int& appliedGlobalSize, unsigned int& appliedLocalSize)
    {
        Vec3ui appliedGlobalSizeVec3, appliedLocalSizeVec3;
        queryAndApplyOptimumWorkSize3D(Vec3ui(minimumGlobalSize, 1, 1), appliedGlobalSizeVec3, appliedLocalSizeVec3);
        appliedGlobalSize = appliedGlobalSizeVec3.x;
        appliedLocalSize = appliedLocalSizeVec3.x;
    }

    void CLKernel::queryAndApplyOptimumWorkSize2D(const Vec2ui& minimumGlobalSize, Vec2ui& appliedGlobalSize, Vec2ui& appliedLocalSize)
    {
        Vec3ui appliedGlobalSizeVec3, appliedLocalSizeVec3;
        queryAndApplyOptimumWorkSize3D(Vec3ui(minimumGlobalSize, 1), appliedGlobalSizeVec3, appliedLocalSizeVec3);
        appliedGlobalSize = appliedGlobalSizeVec3.xy();
        appliedLocalSize = appliedLocalSizeVec3.xy();
    }

    void CLKernel::queryAndApplyOptimumWorkSize3D(const Vec3ui& minimumGlobalSize, Vec3ui& appliedGlobalSize, Vec3ui& appliedLocalSize)
    {
        std::size_t preferredMultiple = 0;
        std::size_t maxLocalSize = 0;
        CL_ASSERT(clGetKernelWorkGroupInfo(clKernel, clal->getDeviceId(), CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(std::size_t), &preferredMultiple, 0));
        CL_ASSERT(clGetDeviceInfo(clal->getDeviceId(), CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(std::size_t), &maxLocalSize, 0));
        calculateWorkSizesForSingleDimension(minimumGlobalSize.x, maxLocalSize, preferredMultiple, appliedGlobalSize.x, appliedLocalSize.x);
        calculateWorkSizesForSingleDimension(minimumGlobalSize.y, maxLocalSize, preferredMultiple, appliedGlobalSize.y, appliedLocalSize.y);
        calculateWorkSizesForSingleDimension(minimumGlobalSize.z, maxLocalSize, preferredMultiple, appliedGlobalSize.z, appliedLocalSize.z);
        setGlobalWorkSize(appliedGlobalSize);
        setLocalWorkSize(appliedLocalSize);
    }

    void CLKernel::calculateWorkSizesForSingleDimension(unsigned int minimumGlobalSize, std::size_t maximumLocalSize, std::size_t preferredLocalSizeMultiple, unsigned int& globalSize, unsigned int& localSize) const
    {
        if(minimumGlobalSize < maximumLocalSize)
        {
            localSize = minimumGlobalSize;
            globalSize = minimumGlobalSize;
        }
        else
        {
            auto localSizeLimitedByMinimum = (unsigned int)(minimumGlobalSize / preferredLocalSizeMultiple) * (unsigned int)preferredLocalSizeMultiple;
            auto localSizeLimitedByMaximum = (unsigned int)(maximumLocalSize / preferredLocalSizeMultiple) * (unsigned int)preferredLocalSizeMultiple;
            localSize = std::min(localSizeLimitedByMinimum, localSizeLimitedByMaximum);
            globalSize = (unsigned int)std::ceil((float)minimumGlobalSize / (float)localSize) * localSize;
        }
    }
}