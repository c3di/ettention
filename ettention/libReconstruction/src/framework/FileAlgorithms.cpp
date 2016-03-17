#include "stdafx.h"

#include "framework/FileAlgorithms.h"

namespace ettention
{
    boost::filesystem::path FileAlgorithms::normalizePath(const boost::filesystem::path &path)
    {
        boost::filesystem::path absPath = absolute(path);
        boost::filesystem::path::iterator it = absPath.begin();
        boost::filesystem::path result = *it++;

        // Get canonical version of the existing part
        for(; exists(result / *it) && it != absPath.end(); ++it)
        {
            result /= *it;
        }
        result = canonical(result);

        // For the rest remove ".." and "." in a path with no symlinks
        for(; it != absPath.end(); ++it)
        {
            // Just move back on ../
            if(*it == "..")
            {
                result = result.parent_path();
            }
            // Ignore "."
            else if(*it != ".")
            {
                // Just cat other path entries
                result /= *it;
            }
        }

        return result;
    }
}