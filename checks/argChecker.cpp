#include <iostream>
#include <string>

namespace IViewer
{
    bool checkArgs(int argc, int inputNumber, bool printError = false)
    {
        try
        {
            if (argc != 2)
            {
                if (argc == 1)
                {
                    throw std::string("Error: No input\n");
                }
                else if (argc > inputNumber)
                {
                    throw std::string("Error: Too many inputs\n");
                }
            }
        }
        catch (std::string e)
        {
            if (printError)
                std::cout << e;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
} // namespace IViewer