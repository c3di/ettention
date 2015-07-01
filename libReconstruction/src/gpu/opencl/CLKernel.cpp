#include "stdafx.h"
#include "CLKernel.h"
#include "CLMemBuffer.h"
#include "CLMemImage.h"
#include "CLMemoryStructure.h"
#include "CLSampler.h"
#include "framework/Framework.h"
#include "setup/ParameterSet/DebugParameterSet.h"
#include "setup/ParameterSet/OptimizationParameterSet.h"

namespace ettention
{
    CLKernel::CLKernel(Framework* framwork, const std::string& caption, const std::string& sourceCode, const std::string& kernelName, const std::string& additionalKernelDefines)
        : clal(framwork->getOpenCLStack())
        , caption(caption)
        , debugKernelParameters(framwork->getParameterSet()->get<DebugParameterSet>()->DisplayKernelParameters())
    {
        getParameterList(sourceCode, kernelName, kernelParameters);
        std::string tempSourceCode = sourceCode;
        fixSourceForImageSupport(tempSourceCode);
        program = generateProgramFromSourceCode(tempSourceCode, kernelName, additionalKernelDefines + getKernelDefines(framwork));
        clKernel = clCreateKernel(program, kernelName.c_str(), &err);
        CL_ASSERT(err);
        gridDim[0] = gridDim[1] = gridDim[2] = 1;
    }

    CLKernel::~CLKernel()
    {
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

    void CLKernel::setArgument(unsigned int paramIndex, CLMemImage* value)
    {
        setArgument(paramIndex, &value->getCLMem(), sizeof(cl_mem));
    }

    void CLKernel::setArgument(unsigned int paramIndex, CLSampler* value)
    {
        setArgument(paramIndex, &value->getCLSampler(), sizeof(cl_mem));
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
            if(value->GetByteWidth() != findIt->second->getByteWidth())
            {
                delete findIt->second;
                findIt->second = new CLMemBuffer(clal, value->GetByteWidth());
            }
            buffer = findIt->second;
        }
        else
        {
            parameterBuffers[paramIndex] = buffer = new CLMemBuffer(clal, value->GetByteWidth());
        }

        std::size_t offset = 0;
        for(unsigned int i = 0; i < value->GetVariableCount(); ++i)
        {
            buffer->transferFrom(value->Get(i)->GetData(), offset, value->Get(i)->GetDataSize());
            offset += value->Get(i)->GetDataSize();
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
        gridDim[0] = x;
        gridDim[1] = y;
        gridDim[2] = z;
    }

    void CLKernel::setGlobalWorkSize(const Vec2ui& xy)
    {
        setGlobalWorkSize(xy.x, xy.y);
    }

    void CLKernel::setGlobalWorkSize(const Vec3ui& xyz)
    {
        setGlobalWorkSize(xyz.x, xyz.y, xyz.z);
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

        const unsigned int dimension = 3;
        cl_event launchEvent;
        CL_ASSERT(clEnqueueNDRangeKernel(clal->getCommandQueue(), clKernel, dimension, NULL, gridDim, 0, 0, NULL, &launchEvent));
        CL_ASSERT(clWaitForEvents(1, &launchEvent));
        clal->profileAndReleaseEvent("Kernel execution: " + caption, launchEvent);
        kernelParameters.getParameterNames(missingArguments);
    }

    cl_kernel CLKernel::getKernel()
    {
        return clKernel;
    }

    void CLKernel::fixSourceForImageSupport(std::string& sourceCode)
    {
        if(clal->HasImageSupport())
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

        if(clal->HasImageSupport())
            source.insert(0, "#define IMAGE_SUPPORT \n");

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
            for(unsigned int i = 0; i < container->GetVariableCount(); ++i)
            {
                container->Get(i)->WriteToStream(std::cout);
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }

    std::string CLKernel::getKernelDefines(Framework* framework)
    {
        auto parameterSet = framework->getParameterSet()->get<OptimizationParameterSet>();
        switch(parameterSet->VoxelRepresentation())
        {
        case VoxelType::HALF_FLOAT_16:
            return " -D HALF_FLOAT_16";
        case VoxelType::FLOAT_32:
            return " -D FLOAT_32";
        default:
            return "";
        }
    }
}