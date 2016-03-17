// compileKernel.cpp : Defines the entry point for the console application.
//

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <unordered_set>
#include <fstream>

using namespace boost::posix_time;

bool isIncludeStatement(std::string line) 
{
    boost::regex ex("(#include\\s+\\\"(.*)\\\")");
    return boost::regex_match(line, ex);
}

std::string extractFileNameFromIncludeStatement(std::string line)
{
    boost::regex ex("(#include\\s+\\\"(.*)\\\")");
    boost::match_results<std::string::const_iterator> match;
    boost::regex_search(line, match, ex);
    return match[2];
}

std::string findIncludeFile(std::string includeFileName, std::ostream& os, std::unordered_set<std::string>& includeSearchDirs)
{
    for(auto it = includeSearchDirs.begin(); it != includeSearchDirs.end(); ++it) 
    {
        boost::filesystem::path pathToFile(*it);
        pathToFile /= includeFileName;
        if(boost::filesystem::exists(pathToFile) && boost::filesystem::is_regular_file(pathToFile))
        {
            boost::filesystem::absolute(pathToFile);
            return pathToFile.string();
        }
    }
    std::cerr << "include file " << includeFileName << " not found, search path was: ";
    for(auto it = includeSearchDirs.begin(); it != includeSearchDirs.end(); ++it) 
        std::cerr << *it << " ";
    std::cerr << std::endl;
    exit(-1);
}

void compile(std::istream& is, std::ostream& ti, std::ostream& os, std::unordered_set<std::string>& includeSearchDirs) 
{
    if(!is.good())
    {
        std::cerr << "interal error opening input file";
        exit(-1);
    }
    while(is.good())
    {
        std::string line;
        std::getline(is, line);
        if(isIncludeStatement(line))
        {
            std::string includeFileName = extractFileNameFromIncludeStatement(line);
            std::string fillFileNameOfIncludeFile = findIncludeFile(includeFileName, os, includeSearchDirs);
            ti << fillFileNameOfIncludeFile << std::endl;
            std::ifstream includeStream(fillFileNameOfIncludeFile);

            compile(includeStream, ti, os, includeSearchDirs);
            includeStream.close();
        }
        else
        {
            os << line << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{
    std::unordered_set<std::string> searchDirs;
    searchDirs.insert(boost::filesystem::current_path().string());

    std::time_t original_time = time(NULL);
    std::time_t compare_time = original_time;
    bool recompile = false;

    std::string ifilename(argv[1]);
    std::string ofilename(argv[2]);
    std::string includesFilename(argv[2]);
    includesFilename += ".includes";

    std::fstream tis;

    boost::filesystem::path pathToIFile(ifilename);
    boost::filesystem::path pathToOFile(ofilename);
    boost::filesystem::path pathToIncludesFile(includesFilename);

    if(!(boost::filesystem::exists(pathToIFile) && boost::filesystem::is_regular_file(pathToIFile)))
    {
        std::cerr << "Input CL file " << ifilename << " not found ";
        exit(-1);
    }

    if(boost::filesystem::exists(pathToOFile) && boost::filesystem::is_regular_file(pathToOFile) && !(boost::filesystem::is_empty(pathToOFile)))
    {
        std::time_t lwt_ifile = boost::filesystem::last_write_time(pathToIFile);
        std::time_t lwt_ofile = boost::filesystem::last_write_time(pathToOFile);
        ptime lwpt_ifile = from_time_t(lwt_ifile);
        ptime lwpt_ofile = from_time_t(lwt_ofile);
        recompile |= (lwpt_ofile < lwpt_ifile);
    }
    else
    {
        recompile = true;
    }

    if(!recompile)
    {
        if(boost::filesystem::exists(pathToIncludesFile) && boost::filesystem::is_regular_file(pathToIncludesFile) && !(boost::filesystem::is_empty(pathToIncludesFile)))
        {
            tis.open(includesFilename, std::ios::in);

            if(!tis.good())
            {
                std::cerr << "Includes file " << includesFilename << " not found ";
                exit(-1);
            }

            std::time_t lwt_ofile = boost::filesystem::last_write_time(pathToOFile);
            ptime lwpt_ofile = from_time_t(lwt_ofile);
            while(tis.good())
            {
                std::string line;
                std::getline(tis, line);
                if(!line.empty())
                {
                    if(boost::filesystem::exists(line) && boost::filesystem::is_regular_file(line) && !(boost::filesystem::is_empty(line)))
                    {
                        std::time_t lwt_include = boost::filesystem::last_write_time(line);
                        ptime lwpt_include = from_time_t(lwt_include);
                        recompile |= (lwpt_ofile < lwpt_include);
                    }
                    else
                    {
                        std::cerr << "CL include file " << line << " not found ";
                        exit(-1);
                    }
                }

            }
            tis.close();
        }
    }

    if(!recompile)
    {
        exit(0);
    }
    else
    {
        std::cout << "generating output, recompilation of " << ofilename << " needed" << std::endl;
    }

    std::ofstream ofs(ofilename);

    for(int i = 3; i < argc; i++)
    {
        std::string arg(argv[i]);
        if(arg == "-I")
        {
            i++;
            std::string dirName(argv[i]);
            searchDirs.insert(dirName);
        }
    }

    std::ifstream ifs(ifilename);
    if(!ifs.good())
    {
        std::cerr << "input file " << ifilename << " not found ";
        exit(-1);
    }
    
    tis.open(includesFilename, std::ios::out);
    compile(ifs, tis, ofs, searchDirs);

    ifs.close();
    tis.close();
    ofs.close();

    exit(0);
    
}

