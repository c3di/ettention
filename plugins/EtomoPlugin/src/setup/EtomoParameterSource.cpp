#include "stdafx.h"
#include "EtomoParameterSource.h"
#include <fstream>
#include <boost/lexical_cast.hpp>
#include "framework/error/Exception.h"

namespace ettention
{
    namespace etomo
    {
        EtomoParameterSource::EtomoParameterSource(std::string pathToSetupFiles, bool addPathToFiles)
            : pathToSetupFiles(pathToSetupFiles)
            , addPathToFiles(addPathToFiles)
            , logLevel(ParameterSource::OVERALL_PROGRESS)
        {
        }

        EtomoParameterSource::~EtomoParameterSource()
        {
        }

        void EtomoParameterSource::parse()
        {
            std::string tiltFileName = pathToSetupFiles + "tilt.com";
            std::string ettentionSetupName = pathToSetupFiles + "ettentionsetup.com";

            if(!boost::filesystem::exists(tiltFileName) || !boost::filesystem::exists(ettentionSetupName))
            {
                return;
            }

            std::ifstream tiltFile(tiltFileName);
            std::ifstream ettentionsetupFile(ettentionSetupName);

            if(!tiltFile.good() || !ettentionsetupFile.good())
            {
                throw Exception("The file tilt.com or ettentionsetup.com could not be opened!");
            }

            useDefaultDeviceType = true;

            addInternalParameters();

            parseTiltFile(tiltFile);
            parseEttentionsetupFile(ettentionsetupFile);

            generateOutputFileName();

            tiltFile.close();
            ettentionsetupFile.close();

            if(useDefaultDeviceType)
            {
                storage.insertOrRewriteScalarParameter<std::string>("hardware.openclDeviceType", "string", "cpu");
            }
        }

        void EtomoParameterSource::parseTiltFile(std::ifstream& tiltFile)
        {
            Vec3ui outputVolumeDimension = Vec3ui(0, 0, 0);

            unsigned int binning = 0;

            int subsetWidth = 0;
            int subsetHeight = 0;

            std::string skipProjectionsList;

            std::string line;
            while(getline(tiltFile, line))
            {
                if(line[0] != '#' && line[0] != '$')
                {
                    std::string paramName;
                    std::string paramValue;
                    separateNameAndValue(line, paramName, paramValue);

                    if(paramName == "IMAGEBINNED")
                    {
                        binning = boost::lexical_cast<unsigned int>(paramValue);
                        storage.insertOrRewriteScalarParameter<unsigned int>("imagebinned", "unsigned int", binning);
                    }
                    else if(paramName == "OutputFile")
                    {
                        outputFileName = getCompletePath(paramValue);
                    }
                    else if(paramName == "InputProjections")
                    {
                        storage.insertOrRewriteScalarParameter<std::string>("input.projections", "string", getCompletePath(paramValue));
                    }
                    else if(paramName == "TILTFILE")
                    {
                        storage.insertOrRewriteScalarParameter<std::string>("input.tiltAngles", "string", getCompletePath(paramValue));
                    }
                    else if(paramName == "THICKNESS")
                    {
                        outputVolumeDimension.z = boost::lexical_cast<unsigned int>(paramValue);
                    }
                    else if(paramName == "MODE")
                    {
                        storage.insertOrRewriteScalarParameter<std::string>("output.options.voxelType", "string", GetVoxelType(paramValue));
                    }
                    else if(paramName == "XAXISTILT")
                    {
                        storage.insertOrRewriteScalarParameter<float>("input.xAxisTilt", "float", boost::lexical_cast<float>(paramValue));
                    }
                    else if(paramName == "FULLIMAGE")
                    {
                        parseDimensions2D("FULLIMAGE", paramValue, outputVolumeDimension.x, outputVolumeDimension.y);
                    }
                    else if((paramName == "EXCLUDELIST") || (paramName == "EXCLUDELIST2"))
                    {
                        if(!skipProjectionsList.empty())
                        {
                            skipProjectionsList += ",";
                        }
                        skipProjectionsList += paramValue;
                    }
                    else if(paramName == "UseGPU")
                    {
                        bool useGPU = boost::lexical_cast<bool>(paramValue);
                        if(useGPU)
                        {
                            storage.insertOrRewriteScalarParameter<std::string>("hardware.openclDeviceType", "string", "gpu_cpu");
                            useDefaultDeviceType = false;
                        }
                    }
                    else if(paramName == "LOG")
                    {
                        storage.insertOrRewriteScalarParameter<bool>("input.logaritmize", "bool", true);
                    }
                }
            }

            if(!skipProjectionsList.empty())
            {
                storage.insertOrRewriteScalarParameter<std::string>("skipProjections", "string", skipProjectionsList);
            }

            binVolumeDimensions(outputVolumeDimension, binning);

            std::stringstream dimensionsToString;
            dimensionsToString << outputVolumeDimension.x << "," << outputVolumeDimension.y << "," << outputVolumeDimension.z;
            storage.insertOrRewriteScalarParameter<std::string>("volume.dimension", "string", dimensionsToString.str());
        }

        void EtomoParameterSource::parseEttentionsetupFile(std::ifstream& sartsetupFile)
        {
            std::string line;
            bool useDefaultInitValue = true;

            while(getline(sartsetupFile, line))
            {
                if(line[0] != '#' && line[0] != '$')
                {
                    std::string paramName;
                    std::string paramValue;
                    separateNameAndValue(line, paramName, paramValue);
                    storage.insertOrRewriteScalarParameter<std::string>(paramName, "string", paramValue);
                    
                    if(paramName == "algorithm.numberOfIterations")
                    {
                        numberOfIterations = paramValue;
                    }
                    else if(paramName == "volume.initFile" || paramName == "volume.initValue")
                    {
                        useDefaultInitValue = false;
                    }
                    else if(paramName == "hardware.openclDeviceType" || paramName == "hardware.openclDeviceId")
                    {
                        useDefaultDeviceType = false;
                    }
                }
            }

            if(useDefaultInitValue)
            {
                storage.insertOrRewriteScalarParameter<float>("volume.initValue", "float", 0.0f);
            }
        }

        void EtomoParameterSource::addInternalParameters()
        {
            storage.insertOrRewriteScalarParameter<bool>("output.options.invert", "bool", true);
            storage.insertOrRewriteScalarParameter<std::string>("output.options.orientation", "string", "xzy_order");
            storage.insertOrRewriteScalarParameter<std::string>("projectionIteration", "string", "maxangle");
        }

        std::string EtomoParameterSource::getCompletePath(std::string fileName)
        {
            std::string completePath;
            if(addPathToFiles)
                completePath = pathToSetupFiles + fileName;
            else
                completePath = fileName;

            return completePath;
        }

        std::string EtomoParameterSource::GetVoxelType(std::string inputValue)
        {
            if(inputValue == "0")
            {
                return "unsigned8";
            }
            else if(inputValue == "6")
            {
                return "unsigned16";
            }
            else if(inputValue == "2")
            {
                return "float32";
            }
            else if(inputValue == "1")
            {
                return "signed16";
            }
            else
            {
                throw Exception("Unknown file mode ");
            }
        }

        void EtomoParameterSource::generateOutputFileName()
        {
            storage.insertOrRewriteScalarParameter("output.filename", "string", outputFileName + "_et" + numberOfIterations);
        }

        void EtomoParameterSource::binVolumeDimensions(Vec3ui& dimensions, unsigned int binning)
        {
            if(binning != 0)
            {
                dimensions.x /= binning;
                dimensions.y /= binning;
                dimensions.z /= binning;
            }
        }

        bool EtomoParameterSource::parameterExists(std::string aName) const
        {
            return storage.parameterExists(aName);
        };

        boost::filesystem::path EtomoParameterSource::getPathParameter(std::string aName) const
        {
            return boost::filesystem::path(getStringParameter(aName));
        };

        std::string EtomoParameterSource::getStringParameter(std::string aName) const
        {
            return storage.getScalarParameter<std::string>(aName);
        };

        float EtomoParameterSource::getFloatParameter(std::string aName) const
        {
            return storage.getScalarParameter<float>(aName);
        }

        bool EtomoParameterSource::getBoolParameter(std::string aName) const
        {
            return storage.getScalarParameter<bool>(aName);
        }

        int EtomoParameterSource::getIntParameter(std::string aName) const
        {
            return storage.getScalarParameter<int>(aName);
        }

        unsigned int EtomoParameterSource::getUIntParameter(std::string aName) const
        {
            return storage.getScalarParameter<unsigned int>(aName);
        }

        Vec3ui EtomoParameterSource::getVec3uiParameter(std::string aName) const
        {
            std::string vectorStringForm = storage.getScalarParameter<std::string>(aName);
            return createVec3uiFromString(vectorStringForm);
        }

        Vec3f EtomoParameterSource::getVec3fParameter(std::string aName) const
        {
            std::string vectorStringForm = storage.getScalarParameter<std::string>(aName);
            return createVec3fFromString(vectorStringForm);
        }


        ParameterSource::LogLevelValues EtomoParameterSource::LogLevel() const
        {
            return logLevel;
        }

        void EtomoParameterSource::LogLevel(const ParameterSource::LogLevelValues level)
        {
            logLevel = level;
        }


        void EtomoParameterSource::separateNameAndValue(std::string& line, std::string& paramName, std::string& paramValue) const
        {
            auto spacePos = line.find_first_of(" \t");
            if(spacePos == std::string::npos)
            {
                throw Exception("Line \"" + line + "\" cannot be split into name / value pair!");
            }
            paramName = line.substr(0, spacePos);
            paramValue = line.substr(spacePos + 1);
        }

        Vec3f EtomoParameterSource::createVec3fFromString(const std::string paramValue) const
        {
            Vec3f floatVector;

            boost::char_separator<char> sep(",");
            boost::tokenizer<boost::char_separator<char> > tokens(paramValue, sep);
            unsigned int i = 0;
            BOOST_FOREACH(std::string t, tokens)
            {
                floatVector[i]=boost::lexical_cast<float>(t);
                i++;
            }

            return floatVector;
        }

        Vec3ui EtomoParameterSource::createVec3uiFromString(const std::string paramValue) const
        {
            Vec3ui uintVector;

            boost::char_separator<char> sep(",");
            boost::tokenizer<boost::char_separator<char> > tokens(paramValue, sep);
            unsigned int i = 0;
            BOOST_FOREACH(std::string t, tokens)
            {
                uintVector[i] = (unsigned int)boost::lexical_cast<float>(t);
                i++;
            }

            return uintVector;
        }
    }
}