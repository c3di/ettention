#pragma once

namespace ettention
{

    class FileAlgorithms
    {
    public:
        static boost::filesystem::path normalizePath(const boost::filesystem::path &path);
    };
}