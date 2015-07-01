#pragma once

namespace ettention
{
    class CLKernelParameterParser
    {
    public:
        struct Parameter
        {
            std::string name;
            unsigned int number;
        };

        class KernelParameters
        {
            friend class CLKernelParameterParser;

        public:
            KernelParameters();
            ~KernelParameters();

            const CLKernelParameterParser::Parameter& getParameterByName(const std::string& name) const;
            const CLKernelParameterParser::Parameter& getParameterByNr(unsigned int nr) const;
            unsigned int getParameterCount() const;
            void getParameterNames(std::unordered_set<std::string>& dest) const;

        private:
            std::unordered_map<std::string, Parameter> parametersByName;
            std::unordered_map<unsigned int, Parameter> parametersByNr;

            void clear();
            void addParameter(const CLKernelParameterParser::Parameter& parameter);
        };

        CLKernelParameterParser(const std::string& srcCode, const std::string kernelName);
        ~CLKernelParameterParser();

        void storeParametersTo(KernelParameters& dest) const;

    private:
        const std::string srcCode;
        const std::string kernelName;

        std::string findKernelParameterString() const;
        void splitParametersAndSaveTo(const std::string& parametersString, std::vector<std::string>& parameters) const;

        static std::string stripCommentsAndBlocks(const std::string& srcCode);
        static std::string stripAllEncapsulatedSubstrings(const std::string& srcCode, const std::string& start, const std::string& end);
        static std::string stripAllBlocks(const std::string& srcCode);
        static Parameter createParameterFromString(unsigned int parameterNr, const std::string& parameterString, unsigned int* parameterNrOffset);
        static void trim(std::string& str);
        static bool isWhiteSpace(char c);
    };
}