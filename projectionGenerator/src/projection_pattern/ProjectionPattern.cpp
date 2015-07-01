#include "stdafx.h"
#include "ProjectionPattern.h"
#include <boost/program_options/errors.hpp>

namespace ettention
{
    namespace stem
    {
        ProjectionPattern::ProjectionPattern()
        {
        }

        ProjectionPattern::~ProjectionPattern()
        {
        }

        PatternType ProjectionPattern::parsePatternType(std::string token)
        {
            if(token == "stack")
                return PatternType::STACK;
            else if(token == "spiral")
                return PatternType::SPIRAL;
            throw boost::program_options::validation_error(boost::program_options::validation_error::invalid_option, token, "pattern");
        }
    }
}