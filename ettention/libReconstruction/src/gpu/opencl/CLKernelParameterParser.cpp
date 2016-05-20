#include "stdafx.h"
#include "CLKernelParameterParser.h"
#include <exception>
#include <sstream>
#include <iostream>

namespace ettention
{
    //////////////////////////////////////////////////////////////////////////
    // KernelParameters
    //////////////////////////////////////////////////////////////////////////

    CLKernelParameterParser::KernelParameters::KernelParameters()
    {
    }
    
    CLKernelParameterParser::KernelParameters::~KernelParameters()
    {
    }

    unsigned int CLKernelParameterParser::KernelParameters::getParameterCount() const
    {
        return (unsigned int)parametersByNr.size();
    }

    void CLKernelParameterParser::KernelParameters::clear()
    {
        parametersByName.clear();
        parametersByNr.clear();
    }

    void CLKernelParameterParser::KernelParameters::getParameterNames(std::unordered_set<std::string>& dest) const
    {
        dest.clear();
        for(auto it = parametersByName.begin(); it != parametersByName.end(); ++it)
        {
            dest.insert(it->first);
        }
    }

    void CLKernelParameterParser::KernelParameters::addParameter(const CLKernelParameterParser::Parameter& parameter)
    {
        auto findByName = parametersByName.find(parameter.name);
        if(findByName != parametersByName.end())
        {
            throw std::runtime_error("Tried to add two parameters with the same name!");
        }
        auto findByNr = parametersByNr.find(parameter.number);
        if(findByNr != parametersByNr.end())
        {
            throw std::runtime_error("Tried to add two parameters to the same nr!");
        }
        parametersByName[parameter.name] = parameter;
        parametersByNr[parameter.number] = parameter;
    }

    const CLKernelParameterParser::Parameter& CLKernelParameterParser::KernelParameters::getParameterByName(const std::string& name) const
    {
        auto findByName = parametersByName.find(name);
        if(findByName == parametersByName.end())
        {
            std::stringstream error;
            error << "Argument with name " << name << " does not exist!";
            throw std::runtime_error(error.str().c_str());
        }
        return findByName->second;
    }

    const CLKernelParameterParser::Parameter& CLKernelParameterParser::KernelParameters::getParameterByNr(unsigned int nr) const
    {
        auto findByNr = parametersByNr.find(nr);
        if(findByNr == parametersByNr.end())
        {
            std::stringstream error;
            error << "Argument with number " << nr << " does not exist!";
            throw std::runtime_error(error.str().c_str());
        }
        return findByNr->second;
    }

    //////////////////////////////////////////////////////////////////////////
    // CLKernelParameterParser
    //////////////////////////////////////////////////////////////////////////

    CLKernelParameterParser::CLKernelParameterParser(const std::string& srcCode, const std::string kernelName)
        : srcCode(stripCommentsAndBlocks(srcCode))
        , kernelName(kernelName)
    {
    }

    CLKernelParameterParser::~CLKernelParameterParser()
    {
    }

    std::string CLKernelParameterParser::stripCommentsAndBlocks(const std::string& srcCode)
    {
        std::string strippedBlocks = stripAllBlocks(srcCode);
        std::string strippedInlineComments = stripAllEncapsulatedSubstrings(strippedBlocks, "//", "\n");
        std::string strippedBlockComments = stripAllEncapsulatedSubstrings(strippedInlineComments, "/*", "*/");
        return strippedBlockComments;
    }

    std::string CLKernelParameterParser::stripAllBlocks(const std::string& srcCode)
    {
        unsigned int nestingLevel = 0;
        std::string stripped = "";
        unsigned int pos = (unsigned int)-1;
        unsigned int codeStart = 0;
        while(++pos < srcCode.size())
        {
            if(srcCode[pos] == '{')
            {
                if(nestingLevel == 0)
                {
                    stripped += srcCode.substr(codeStart, pos - codeStart);
                }
                ++nestingLevel;
            }
            else if(srcCode[pos] == '}')
            {
                --nestingLevel;
                if(nestingLevel < 0)
                {
                    throw std::runtime_error("Illegal source code given!");
                }
                if(nestingLevel == 0)
                {
                    codeStart = pos + 1;
                }
            }
        }
        if(nestingLevel != 0)
        {
            throw std::runtime_error("Illegal source code given!");
        }
        if(codeStart < srcCode.size())
        {
            stripped += srcCode.substr(codeStart, pos - codeStart);
        }
        return stripped;
    }

    std::string CLKernelParameterParser::stripAllEncapsulatedSubstrings(const std::string& srcCode, const std::string& start, const std::string& end)
    {
        std::string::size_type substringStart = srcCode.find(start);
        std::string::size_type substringEnd = srcCode.find(end, substringStart);

        if(substringStart == std::string::npos || substringEnd == std::string::npos)
        {
            return srcCode;
        }
        else
        {
            std::string stripped = srcCode.substr(0, substringStart);
            stripped += srcCode.substr(substringEnd + end.size());
            return stripAllEncapsulatedSubstrings(stripped, start, end);
        }
    }

    void CLKernelParameterParser::storeParametersTo(KernelParameters& dest) const
    {
        dest.clear();
        std::string kernelArgsString = this->findKernelParameterString();
        std::vector<std::string> parameters;
        this->splitParametersAndSaveTo(kernelArgsString, parameters);

        unsigned int parameterNrOffset = 0;
        for(unsigned int i = 0; i < parameters.size(); ++i)
        {
            Parameter param = createParameterFromString(i, parameters[i], &parameterNrOffset);
            dest.addParameter(param);
        }
    }

    std::string CLKernelParameterParser::findKernelParameterString() const
    {
        std::string::size_type searchStartPos = 0;
        while(searchStartPos < srcCode.size())
        {
            std::string::size_type kernelNameStartPos = srcCode.find(kernelName, searchStartPos);
            std::string::size_type kernelNameEndPos = kernelNameStartPos + kernelName.size();
            if(kernelNameStartPos == std::string::npos)
            {
                std::stringstream error;
                error << "Kernel " << kernelName << " does not exist in given source code!";
                throw std::runtime_error(error.str().c_str());
            }
            std::string::size_type bracketPos = srcCode.find('(', kernelNameEndPos);
            if(bracketPos == std::string::npos)
            {
                break;
            }
            bool found = true;
            for(std::string::size_type i = kernelNameEndPos; i < bracketPos; ++i)
            {
                if(srcCode[i] != ' ' && srcCode[i] != '\n')
                {
                    found = false;
                    searchStartPos = kernelNameStartPos + 1;
                }
            }
            if(found)
            {                
                std::string::size_type closingBracketPos = srcCode.find(')', bracketPos);
                while((closingBracketPos != std::string::npos) && (closingBracketPos + 1) < srcCode.size())
                {                    
                    if(srcCode[closingBracketPos + 1] == ',')
                    {
                        closingBracketPos = srcCode.find(')', closingBracketPos + 1);
                    }
                    else
                    {
                        break;
                    }                    
                }
                if(closingBracketPos != std::string::npos)
                {
                    return srcCode.substr(bracketPos + 1, closingBracketPos - bracketPos - 1);
                }
                else
                {
                    break;
                }
            }
        }
        return "";
    }

    void CLKernelParameterParser::splitParametersAndSaveTo(const std::string& parametersString, std::vector<std::string>& parameters) const
    {
        std::string::size_type pos = 0;
        std::string::size_type commaPos = 0;
        while((commaPos = parametersString.find(',', pos)) != std::string::npos)
        {
            std::string param = parametersString.substr(pos, commaPos - pos);
            trim(param);
            if(!param.empty())
            {
                parameters.push_back(param);
            }
            pos = commaPos + 1;
        }
        std::string lastParam = parametersString.substr(pos);
        trim(lastParam);
        if(!lastParam.empty())
        {
            parameters.push_back(lastParam);
        }
    }

    CLKernelParameterParser::Parameter CLKernelParameterParser::createParameterFromString(unsigned int parameterNr, const std::string& parameterString, unsigned int* parameterNrOffset)
    {
        parameterNr += *parameterNrOffset;
        std::string::size_type paramNamePos = parameterString.find_last_of(' ');
        if(paramNamePos == std::string::npos)
        {
            paramNamePos = parameterString.find('(');
            if(paramNamePos == std::string::npos)
            {
                throw std::runtime_error(("Invalid parameter: " + parameterString + "!").c_str());
            }

            if( requireAdditionalParameterForSampler(parameterString.substr(0, paramNamePos) ) )
                *parameterNrOffset = *parameterNrOffset + 1;
        }
        ++paramNamePos;
        while(paramNamePos < parameterString.size() && parameterString[paramNamePos] == '*')
        {
            ++paramNamePos;
        }
        std::string paramName = parameterString.substr(paramNamePos);
        trim(paramName);
        if(paramName[paramName.size() - 1] == ')')
        {
            paramName = paramName.substr(0, paramName.size() - 1);
        }
        if(paramName.size() == 0)
        {
            throw std::runtime_error(("Invalid parameter: " + parameterString + "!").c_str());
        }
        Parameter param = { paramName, parameterNr, };
        return param;
    }

    bool CLKernelParameterParser::requireAdditionalParameterForSampler(const std::string& parameterString)
    {
        static const std::string TYPE_2D_RO{"TYPE_2D_READONLY"};
        static const std::string TYPE_3D_RO{"TYPE_3D_READONLY"};
        static const std::string TYPE_2D_MASK{"TYPE_2D_MASK"};
        static const std::string TYPE_3D_MASK{"TYPE_3D_MASK"};

        if( (parameterString == TYPE_2D_RO) || (parameterString == TYPE_3D_RO) || (parameterString == TYPE_2D_MASK) || (parameterString == TYPE_3D_MASK) )
            return true;

        return false;
    }

    void CLKernelParameterParser::trim(std::string& str)
    {
        std::string::size_type pos = 0;
        while(pos < str.size() && isWhiteSpace(str[pos]))
        {
            ++pos;
        }
        str = str.substr(pos);
        if(!str.empty())
        {
            pos = str.size() - 1;
            while(pos >= 0 && isWhiteSpace(str[pos]))
            {
                --pos;
            }
            str = str.substr(0, pos + 1);
            for(unsigned int i = 0; i < str.size(); ++i)
            {
                if(isWhiteSpace(str[i]))
                {
                    str[i] = ' ';
                }
            }
        }
    }

    bool CLKernelParameterParser::isWhiteSpace(char c)
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

}