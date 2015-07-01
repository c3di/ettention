#pragma once
#include "gpu/opencl/CLAbstractionLayer.h"
#include "gpu/opencl/CLKernelParameterParser.h"
#include "gpu/variables/VariableContainer.h"
#include "framework/math/Vec3.h"
#include "framework/math/Vec4.h"
#include "framework/math/Matrix4x4.h"
#include <unordered_set>

namespace ettention
{
    class CLMemBuffer;
    class CLMemImage;
    class CLSampler;
    class Framework;

    class CLKernel
    {
    public:
        CLKernel(Framework* framwork, const std::string& caption, const std::string& sourceCode, const std::string& kernelName, const std::string& additionalKernelDefines = "");
        ~CLKernel();

        void setGlobalWorkSize(unsigned int x, unsigned int y = 1, unsigned int z = 1);
        void setGlobalWorkSize(const Vec2ui& xy);
        void setGlobalWorkSize(const Vec3ui& xyz);

        void run();
        
        cl_kernel getKernel();

        void allocateArgument(const std::string& paramName, size_t paramSize);
        void setArgument(unsigned int paramIndex, const void* rawData, size_t rawSize);
        void setArgument(unsigned int paramIndex, VariableContainer* value);
        void setArgument(unsigned int paramIndex, CLMemoryStructure* value);
        void setArgument(unsigned int paramIndex, CLMemBuffer* value);
        void setArgument(unsigned int paramIndex, CLMemImage* value);
        void setArgument(unsigned int paramIndex, CLSampler* value);

        template<typename _ValueType>
        void setArgument(unsigned int paramIndex, GPUMapped<_ValueType>* value);

        template<typename _ValueType>
        void setArgument(unsigned int paramIndex, const _ValueType& value);

        template<typename _ValueType>
        void setArgument(const std::string& paramName, _ValueType value);

    private:
        CLAbstractionLayer* clal;
        std::string caption;
        cl_program program;
        cl_kernel clKernel;
        size_t gridDim[3];
        int kernelIndex;
        cl_int err;
        bool debugKernelParameters;
        CLKernelParameterParser::KernelParameters kernelParameters;
        std::unordered_set<std::string> missingArguments;
        std::unordered_map<unsigned int, CLMemBuffer*> parameterBuffers;

        static std::string getKernelDefines(Framework* framework);
        void fixSourceForImageSupport(std::string& sourceCode);
        std::string getSource(const std::string& fileName);
        std::string handleIncludeStatements(std::string& source);
        cl_program generateProgramFromSourceCode(const std::string& sourceCode, const std::string& kernelName, const std::string& additionalDefines);
        void getParameterList(const std::string& sourceCode, const std::string& kernelName, CLKernelParameterParser::KernelParameters& kernelParameters);
        int getParameterNumber(const std::string& parameterName);
        void outputVariablesIfRequested(const VariableContainer* container);
    };

    template<typename _ValueType>
    void CLKernel::setArgument(unsigned int paramIndex, const _ValueType& value)
    {
        if(sizeof(_ValueType) == 12) // Vec3* cases, fill 4th component with 0
        {
            char data[16];
            memcpy(data, &value, 12);
            memset(data + 12, 0, 4);
            this->setArgument(paramIndex, data, 16);
        }
        else
        {
            this->setArgument(paramIndex, &value, sizeof(_ValueType));
        }
    }

    template<typename _ValueType>
    void CLKernel::setArgument(unsigned int paramIndex, GPUMapped<_ValueType>* value)
    {
        value->ensureIsUpToDateOnGPU();
        this->setArgument(paramIndex, value->getBufferOnGPU());
    }

    template<typename _ValueType>
    void CLKernel::setArgument(const std::string& paramName, _ValueType value)
    {
        setArgument(kernelParameters.getParameterByName(paramName).number, value);
        missingArguments.erase(paramName);
    }
}