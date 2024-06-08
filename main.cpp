#include <iostream>
#include <fmt/ranges.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <fmt/ostream.h>
#include <filesystem>
#include <fmt/chrono.h>
#include <chrono>
//https://www.youtube.com/watch?v=NcEE5xmpgQ0

auto toDecimal(int n) -> int
{
    int dec = 0, i = 0, rem;

    while (n!=0) {
        rem = n % 10;
        n /= 10;
        dec += rem * pow(2, i);
        ++i;
    }

    return dec;
}

//https://stackoverflow.com/questions/22192254/reading-binary-data-into-struct-with-ifstream

struct BMP {
#pragma pack(push, 1)
    struct FILEHEADER {
        char bfType[2];
        int32_t bfSize;
        int16_t bfReserved;
        int16_t bfReserved2;
        int32_t bfOffBits;
    } fh;
#pragma pack(pop)

#pragma pack(push, 1)
    struct FILEINFOHEADER {
        int32_t biSize;
        int32_t biWidth;
        int32_t biHeight;
        int16_t biPlanes;
        int16_t biBitCount;
        int32_t biCompression;
        int32_t biSizeImage;
        int32_t biXPelsPerMeter;
        int32_t biYPelsPerMeter;
        int32_t biClrUsed;
        int32_t biClrImportant;
    } fih;
#pragma pack(pop)

private:
    long long fileSize;
    std::vector<std::vector<unsigned char>> pixelsArray;
    unsigned long long messLen;
    int width;
    int height;

public:
    auto readFile(std::string const &path) {
        auto file = std::fstream(path, std::ios::in | std::ios::binary);

        if (!file.is_open()) {
            fmt::println("Cannot open a image");
            exit(EXIT_FAILURE);
        }

        file.read((char*) &fh, sizeof(FILEHEADER));
        file.read((char*) &fih, sizeof(FILEINFOHEADER));

        if (fh.bfType[0] != 'B' && fh.bfType[1] != 'M') {
            fmt::println("The file is not BMP format");
            exit(EXIT_FAILURE);
        }

        width = fih.biWidth;
        height = fih.biHeight;

        for (int i = 0; i < fih.biHeight; ++i) {
            for (int j = 0; j < fih.biWidth; ++j) {
                auto pixels = std::vector<unsigned char>{0, 0, 0};
                file.read(reinterpret_cast<char *>(pixels.data()), 3);
                pixelsArray.push_back(pixels);
            }
        }
        fileSize = file.tellg(); //size from image can be replaced by any value
    }

    auto writeFile() {
        auto file = std::fstream("newFile.bmp", std::ios::out | std::ios::binary);
        file.write((char *) &fh, sizeof(FILEHEADER));
        file.write((char *) &fih, sizeof(FILEINFOHEADER));

        for (std::vector<unsigned char> c: pixelsArray) {
            file.write(reinterpret_cast<char *>(c.data()), 3);
        }
        file.close();
    }

    auto encode(std::string txt) {
        auto binaryText = std::string();
        txt+='$';
        for (auto ch: txt) {
            binaryText += std::bitset<8>(ch).to_string();
        }

        auto idx = 0;
        messLen = binaryText.length();
        for ( auto &pixel: pixelsArray) {
            for (auto &color: pixel) {
                if (idx < binaryText.length()) {
                    auto binC = std::bitset<8>(color).to_string();
                    binC.back() = binaryText[idx];
                    color = toDecimal(stoi(binC));
                    ++idx;
                }
            }
        }
    }

    auto decode() -> std::string {
        auto text = std::string();
        auto temp = std::string();
        auto bits = 0;
        auto flag = false;

        for (auto const &pixel: pixelsArray) {
            for (auto color: pixel) {
                if (bits == 8) {
                    text += (char) toDecimal(stoi(temp));
                    temp = "";
                    bits = 0;
                }
                auto binC = std::bitset<8>(color).to_string();
                temp += binC.back();
                bits++;
                if(text.back() == '$') {
                    flag = true;
                }
            }
            if(flag)
                break;
        }

        text.pop_back();
        return text;
    }

    auto getFileSize() const -> long long {
        return fileSize;
    }

    auto getMessLen() const -> unsigned long long
    {
        return messLen;
    }

    auto getImgWidth() const -> int
    {
        return width;
    }
    auto getImgHeight() const ->int
    {
        return height;
    }
};

struct PNG
{
#pragma pack(push,1)
    struct HEADER
    {
        uint8_t ch;
        char type[3];
        uint16_t doseof;
        uint8_t wineof;
        uint8_t conud;
    } h;
#pragma pack(pop)
private:
    int fileSize;
    unsigned long long messLen;
    std::vector<std::vector<unsigned char>> pixelsArray;
public:
    auto readFile(std::string const& path)
    {
        auto file = std::fstream(path, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            fmt::println("Cannot open a image");
            exit(EXIT_FAILURE);
        }

//        file.read((char*)&h, sizeof(HEADER));
//        fmt::println("{}", h.ch);
//        fmt::println("{}", h.type);
//        fmt::println("{}", h.doseof);
//        fmt::println("{}", h.wineof);
//        fmt::println("{}", h.conud);
        if(h.type[0] != 'P' && h.type[1] != 'N' && h.type[2]!= 'G')
        {
            fmt::println("The file is not PNG format");
            exit(EXIT_FAILURE);
        }


    }
    auto writeFile()
    {

    }
    auto encode(std::string& txt)
    {

    }
    auto decode()
    {

    }
    auto getFileSize() -> int
    {
        return fileSize;
    }
    auto getMessLen() -> unsigned long long
    {
        return messLen;
    }
};

struct PPM
{
private:
    long long fileSize;
    unsigned long long messLen;
    std::vector<std::string> vec;
    std::string pathFile;
    std::string height_width;

public:
    auto readFile(std::string const &path)
    {
        auto file = std::fstream(path, std::ios::in);
        fileSize = file.tellg();
        pathFile = path;

        if (!file.is_open()) {
            fmt::println("Cannot open an image");
            exit(EXIT_FAILURE);
        }

        auto word = std::string();
        while(std::getline(file,word)) {
                vec.push_back(word);
        }
        height_width = vec[2];

        if(vec[0] != "P3")
        {
            fmt::println("The file is not PPM format");
            exit(EXIT_FAILURE);
        }
//        struct stat res;
//        if(stat(file.c_str, &res) == 0)
//        {
//
//        }
    }

    auto writeFile()
    {
        auto file = std::fstream("newFile.ppm", std::ios::out);
        for(auto const &a: vec)
        {
            file << a << "\n";
        }
    }

    auto encode(std::string txt)
    {
        auto binaryText = std::string();
        txt+='$';
        for (auto ch: txt) {
            binaryText += std::bitset<8>(ch).to_string();
        }

        auto idx = 0;
        messLen = binaryText.length();

        for(int i = 4; i < vec.size(); i++)
        {
            std::string num;
            for(int j = 0; j <=  vec[i].size(); j++)
            {

                if(isdigit(vec[i][j]))
                {
                    num+=vec[i][j];
                }
                else
                {
                    if(!num.empty()) {
                        if (idx < binaryText.length()) {
                            int count = num.size();
                            auto binC = std::bitset<8>(std::stoi(num)).to_string();
                            binC.back() = binaryText[idx];
                            num = std::to_string(toDecimal(stoi(binC)));

                            for(auto a: num)
                            {
                                vec[i][j-count] = a;
                                count--;
                            }
                            ++idx;
                        }
                    }
                    num = "";
                }
            }
        }
    }

    auto decode()
    {
        auto text = std::string();
        auto temp = std::string();
        auto bits = 0;
        auto flag = false;

        for(int i = 4; i < vec.size(); i++)
        {
            std::string num;
            for(int j = 0; j <=  vec[i].size(); j++)
            {
                if(isdigit(vec[i][j]))
                {
                    num+=vec[i][j];
                }
                else
                {
                    if(!num.empty()) {
                        if (bits == 8) {
                            text += (char) toDecimal(stoi(temp));
                            temp = "";
                            bits = 0;
                        }
                        auto binC = std::bitset<8>(std::stoi( num)).to_string();
                        temp += binC.back();
                        bits++;
                    }
                    if(text.back() == '$') {
                        flag = true;
                    }
                    num = "";
                }
            }
            if(flag)
                break;
        }

        text.pop_back();
        text.pop_back();
        return text;
    }

    auto getMesslen() const -> unsigned  long long
    {
        return messLen;
    }

    auto getFileSize() const -> unsigned long long
    {
        auto file = std::filesystem::path(pathFile);
        return std::filesystem::file_size(file);
    }

    auto getHeightWidth()const -> std::string
    {
        return height_width;
    }

    auto getLastModDate()
    {
        auto file = std::filesystem::path(pathFile);
        auto time = std::filesystem::last_write_time(file);
        return time;
    }
};

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

    //fmt::println("{}",argv[1]);
//    auto path = std::string("C:\\Users\\Krystian\\Desktop\\Studia\\pjc\\steganography_pjc\\snail.bmp");
//    auto path = std::string("C:\\Users\\Krystian\\Desktop\\Studia\\pjc\\steganography_pjc\\test.bmp");
//    auto path = std::string("C:\\Users\\Krystian\\Desktop\\Studia\\pjc\\steganography_pjc\\sines.ppm");
//    auto bmp = BMP();
//    bmp.readFile(path);
//    bmp.encode("basia");
//    bmp.writeFile();
//    auto path = std::string("C:\\Users\\Krystian\\Desktop\\Studia\\pjc\\steganography_pjc\\cmake-build-debug\\newFile.bmp");
//    auto path = std::string("C:\\Users\\Krystian\\Desktop\\Studia\\pjc\\steganography_pjc\\cmake-build-debug\\newFile.ppm");
//    bmp.readFile(path);
//    fmt::println("{}", bmp.decode());
//        auto ppm = PPM();
//        ppm.readFile(path);
//        ppm.encode("basia");
//        ppm.writeFile();
//       fmt::println("{}", ppm.decode());
//    fmt::println("{}",argv[1]);
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
                fmt::println("File resolution: {} {}",  bmp.getImgWidth(), bmp.getImgHeight());
            }
            else if(checkFormat(argv[2]) == format::PPM)
            {
                auto ppm = PPM();
                ppm.readFile(argv[2]);
                fmt::println("File size: {} B", ppm.getFileSize());
                fmt::println("File resolution: {}", ppm.getHeightWidth());
//                fmt::println("Last modified: {}", ppm.getLastModDate());
            }
        }
        else
        {
            fmt::println("Given format is not supported");
        }

    }
    else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "-encrypt") == 0)
    {
        if(argc >4)
        {
            fmt::println("Too many arguments");
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
