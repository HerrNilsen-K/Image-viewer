#include "fileChecker.hpp"
#include <fstream>

namespace IViewer
{
    bool filExists(const char *p_file)
    {
        std::ifstream inputFile;
        inputFile.open(p_file, std::ifstream::in);
        return inputFile.fail() ? false : true;
    }
} // namespace IViewer