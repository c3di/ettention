#pragma once

#include "setup/parametersource/ParameterSource.h"
#include "setup/ParameterStorage.h"
#include "framework/Logger.h"

namespace ettention
{
    namespace etomo
    {
        class PLUGIN_API EtomoParameterSource : public ParameterSource
        {
        public:
            EtomoParameterSource(std::string pathToSetupFiles, bool addPathToFiles = false);
            ~EtomoParameterSource();

            void parse() override;

            bool parameterExists(std::string aName) const override;
            boost::filesystem::path getPathParameter(std::string aName) const override;
            std::string getStringParameter(std::string aName) const override;
            float getFloatParameter(std::string aName) const override;
            bool getBoolParameter(std::string aName) const override;
            int getIntParameter(std::string aName) const override;
            unsigned int getUIntParameter(std::string aName) const override;
            Vec3ui getVec3uiParameter(std::string aName) const override;
            Vec3f getVec3fParameter(std::string aName) const override;

            ParameterSource::LogLevelValues LogLevel() const override;
            void LogLevel(const ParameterSource::LogLevelValues level) override;

        protected:

            ParameterStorage storage;
            ParameterSource::LogLevelValues logLevel;

            std::string outputFileName;
            std::string numberOfIterations;
            bool useDefaultDeviceType;

        private:

            std::string pathToSetupFiles;
            bool addPathToFiles;

            std::string getCompletePath(std::string fileName);
            void parseTiltFile(std::ifstream& tiltFile);
            void parseEttentionsetupFile(std::ifstream& sartsetupFile);
            std::string GetVoxelType(std::string inputValue);
            void addInternalParameters();
            void binVolumeDimensions(Vec3ui& dimensions, unsigned int binning);
            void generateOutputFileName();

            template<typename tOutputType>
            void parseDimensions2D(std::string aParamName, std::string aParamValue, tOutputType& aWidth, tOutputType& aHeight)
            {
                boost::char_separator<char> sep(" ");
                boost::tokenizer<boost::char_separator<char> >  tok(aParamValue, sep);
                unsigned int numTokens = 0;
                for(boost::tokenizer<boost::char_separator<char> >::iterator it = tok.begin(); it != tok.end(); it++)
                {
                    numTokens++;
                }

                boost::tokenizer<boost::char_separator<char> >::iterator tokenIter = tok.begin();
                switch(numTokens)
                {
                case 0:
                    LOGGER_IMP(aParamName + " parameter found but no values exist or could not be parsed");
                    break;
                case 1:
                    aWidth = aHeight = boost::lexical_cast<tOutputType>(*tokenIter);
                    LOGGER_IMP(std::string("Only one dimension found in ") + aParamName + " parameter. Setting width and height equal to this one");
                    break;
                case 2:
                    aWidth = boost::lexical_cast<tOutputType>(*tokenIter++);
                    aHeight = boost::lexical_cast<tOutputType>(*tokenIter);
                    break;
                default:
                    aWidth = boost::lexical_cast<tOutputType>(*tokenIter++);
                    aHeight = boost::lexical_cast<tOutputType>(*tokenIter);
                    LOGGER_IMP(std::string("More than two values found in ") + aParamName + " parameter. Using first one as width, second one as height, ignoring the rest.");
                }
            }

            void separateNameAndValue(std::string& line, std::string& paramName, std::string& paramValue) const;
            Vec3f createVec3fFromString(const std::string paramValue) const;
            Vec3ui createVec3uiFromString(const std::string paramValue) const;
        };
    }
}
