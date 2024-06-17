#include <iostream>
#include <fmt/ranges.h>
#include <string>
#include <algorithm>
#include <chrono>
#include "formats.h"

enum class format
{
    BMP, PPM
};

auto checkFormat(std::string const &path) -> format
{
    auto end = path.end()-4;
    if(path.substr(path.find(*end),4) == ".bmp")
    {
        return format::BMP;
    }

    else if(path.substr(path.find(*end),4) == ".ppm")
    {
        return format::PPM;
    }
}

auto checkExt(std::string const &path) -> bool
{
    auto end = path.end()-4;
    if(path.substr(path.find(*end),4) == ".bmp")
    {
        return true;
    }
    else if(path.substr(path.find(*end),4) == ".ppm")
    {
        return true;
    }
    return false;
}

auto main(int argc, char** argv) -> int {

    if(strcmp(argv[1],"-i") == 0 || strcmp(argv[1], "-info") == 0)
    {
        if(argc >3)
        {
            fmt::println("Too many arguments");
            exit(EXIT_FAILURE);
        }
        if(checkExt(argv[2])) {
            if(checkFormat(argv[2]) == format::BMP)
            {
                auto bmp = BMP();
                bmp.readFile(argv[2]);
                fmt::println("File size: {} B", bmp.getFileSize());
                fmt::println("File resolution: {} {}",  bmp.getImgWidth(), bmp.getImgHeight());
                std::cout << std::format("File write time is {}\n", bmp.getLastModDate());  //https://en.cppreference.com/w/cpp/filesystem/last_write_time
            }
            else if(checkFormat(argv[2]) == format::PPM)
            {
                auto ppm = PPM();
                ppm.readFile(argv[2]);
                fmt::println("File size: {} B", ppm.getFileSize());
                fmt::println("File resolution: {}", ppm.getHeightWidth());
                std::cout << std::format("File write time is {}\n", ppm.getLastModDate());
            }
        }
        else
        {
            fmt::println("Given format is not supported");
        }

    }
    else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "-encrypt") == 0)
    {
        if(argc > 4)
        {
            fmt::println("Too many arguments");
            exit(EXIT_FAILURE);
        }
        if(argc < 4)
        {
            fmt::println("Too less arguments");
            exit(EXIT_FAILURE);
        }

        if(checkExt(argv[2])) {
            if(checkFormat(argv[2]) == format::BMP)
            {
                auto bmp = BMP();
                bmp.readFile(argv[2]);
                bmp.encode(argv[3]);
                bmp.writeFile();
            }
            else if(checkFormat(argv[2]) == format::PPM)
            {
                auto ppm = PPM();
                ppm.readFile(argv[2]);
                ppm.encode(argv[3]);
                ppm.writeFile();
            }
        }
        else
        {
            fmt::println("Given format is not supported");
        }
    }
    else if (strcmp(argv[1], "-d") == 0|| strcmp(argv[1], "-decrypt") == 0)
    {
        if(argc > 3)
        {
            fmt::println("Too many arguments");
            exit(EXIT_FAILURE);
        }
        if(argc < 3)
        {
            fmt::println("Too less arguments");
            exit(EXIT_FAILURE);
        }

        if(checkExt(argv[2])) {
            if(checkFormat(argv[2]) == format::BMP)
            {
                auto bmp = BMP();
                bmp.readFile(argv[2]);
                fmt::println( "{}",bmp.decode());
            }
            else if(checkFormat(argv[2]) == format::PPM)
            {
                auto ppm = PPM();
                ppm.readFile(argv[2]);
                fmt::println( "{}",ppm.decode());
            }
        }
        else
        {
            fmt::println("Given format is not supported");
        }
    }
    else if (strcmp(argv[1], "-c") == 0|| strcmp(argv[1], "-check") == 0)
    {
        if(argc >3)
        {
            fmt::println("Too many arguments");
            exit(EXIT_FAILURE);
        }
        if(argc < 3)
        {
            fmt::println("Too less arguments");
            exit(EXIT_FAILURE);
        }

        if(checkExt(argv[2])) {
            if(checkFormat(argv[2]) == format::BMP)
            {
                auto bmp = BMP();
                bmp.readFile(argv[2]);
                if(bmp.getMessLen() < bmp.getFileSize())
                {
                    fmt::println("Message can be written in the file");
                }
                else
                {
                    fmt::println("Message cannot be written in the file");
                }
                if(bmp.checkMess())
                {
                    fmt::println("Hidden message can be in that image");
                }
                else
                {
                    fmt::println("Hidden message cannot be in that image");
                }
            }
            else if(checkFormat(argv[2]) == format::PPM)
            {
                auto ppm = PPM();
                ppm.readFile(argv[2]);
                if(ppm.getMesslen() < ppm.getFileSize())
                {
                    fmt::println("Message can be written in the file");
                }
                else
                {
                    fmt::println("Message cannot be written in the file");
                }
                if(ppm.checkMess())
                {
                    fmt::println("Hidden message can be in that image");
                }
                else
                {
                    fmt::println("Hidden message cannot be in that image");
                }
            }
        }
        else
        {
            fmt::println("Given format is not supported");
        }
    }

    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0)
    {
        if(argc >2)
        {
            fmt::println("Too many arguments");
            exit(EXIT_FAILURE);
        }
        if(argc < 2)
        {
            fmt::println("Too less arguments");
            exit(EXIT_FAILURE);
        }

        fmt::println("NAME:\n Encode or decode message in image \n  \n"
                     "OPTIONS "
                     "\n -i or -info   -> show informations about image(size, resolution, date of last modified the file)"
                     "\n -e or -encode -> encode message in image "
                     "\n -d or -decode -> decode hidden message from image"
                     "\n -c or -check  -> check if you can hide a massage in image"
                     "\n -h or -help   -> show informations about program"
                     "\n \nUSAGE"
                     "\n -i or -info   -> [path to image]"
                     "\n -e or -encode -> [path to image] [message] "
                     "\n -d or -decode -> [path to image]"
                     "\n -c or -check  -> [path to image]"
                     "\n -h or -help   -> [path to image]");
    }
}
