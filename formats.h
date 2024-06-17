#include <iostream>
#include <fmt/ranges.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
#include <fmt/ostream.h>
#include <filesystem>
#include <chrono>

#ifndef STEGANOGRAPHY_PJC_FORMATS_H
#define STEGANOGRAPHY_PJC_FORMATS_H

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
    std::string pathFile;

public:
    auto readFile(std::string const &path) {
        if(access(path.c_str(), R_OK) != 0)
        {
            fmt::println("You don't have read access");
            exit(EXIT_FAILURE);
        }
        pathFile = path;
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
        auto lastPos = pathFile.find_last_of("\\");
        pathFile.replace(lastPos + 1, std::string::npos, "encodedImg.bmp"); //std::string::npos:  replace all chars from last pos to end of the string
        auto file = std::fstream(pathFile, std::ios::out | std::ios::binary);
        file.write((char *) &fh, sizeof(FILEHEADER));
        file.write((char *) &fih, sizeof(FILEINFOHEADER));

        for (std::vector<unsigned char> c: pixelsArray) {
            file.write(reinterpret_cast<char *>(c.data()), 3);
        }
        file.close();
    }

    auto encode(std::string txt) {
        auto binaryText = std::string();
        txt = txt.append("{pjc}");
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

                if(text.size() > 5) {
                    auto end = text.end() - 5;
                    if (text.substr(text.find(*end), 5) == "{pjc}") {
                        flag = true;
                    }
                }
            }
            if(flag)
                break;
        }
        if(flag) {
            text.erase(text.end() - 5, text.end());
            return text;
        }
        else {
            return "Error cannot decode message";
        }
    }

    auto checkMess() -> bool {
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
                if(text.size() > 5) {
                    auto end = text.end() - 5;
                    if (text.substr(text.find(*end), 5) == "{pjc}") {
                        flag = true;
                    }
                }
            }
            if(flag)
                break;
        }
        return flag;
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

    auto getLastModDate()
    {
        using namespace std::chrono_literals; //https://en.cppreference.com/w/cpp/filesystem/last_write_time
        auto file = std::filesystem::path(pathFile);
        auto time = std::filesystem::last_write_time(file);
        std::filesystem::last_write_time(file, time + 2h);
        return time;
    }
};

struct PPM
{
private:
    unsigned long long messLen;
    std::vector<std::string> vec;
    std::string pathFile;
    std::string height_width;

public:
    auto readFile(std::string const &path)
    {
        if(access(path.c_str(), R_OK) != 0)
        {
            fmt::println("You don't have read access");
            exit(EXIT_FAILURE);
        }

        auto file = std::fstream(path, std::ios::in);
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
    }

    auto writeFile()
    {
        auto lastPos = pathFile.find_last_of("\\");
        pathFile.replace(lastPos + 1, std::string::npos, "encodedImg.ppm"); //std::string::npos:  replace all chars from last pos to end of the string
        auto file = std::fstream(pathFile, std::ios::out);
        for(auto const &a: vec)
        {
            file << a << "\n";
        }
    }

    auto encode(std::string txt)
    {
        auto binaryText = std::string();
        txt = txt.append("{pjc}");
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

    auto decode() -> std::string
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
                    if(text.size() > 5) {
                        auto end = text.end() - 5;
                        if (text.substr(text.find(*end), 5) == "{pjc}") {
                            flag = true;
                        }
                    }
                    num = "";
                }
            }
            if(flag)
                break;
        }
        if(flag) {
            text.erase(text.end() - 6, text.end());
            return text;
        }
        else
        {
            return "Error cannot decode message";
        }
    }

    auto checkMess()-> bool
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
                    if(text.size() > 5) {
                        auto end = text.end() - 5;
                        if (text.substr(text.find(*end), 5) == "{pjc}") {
                            flag = true;
                        }
                    }
                    num = "";
                }
            }
            if(flag)
                break;
        }

        return flag;
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
        using namespace std::chrono_literals; //https://en.cppreference.com/w/cpp/filesystem/last_write_time
        auto file = std::filesystem::path(pathFile);
        auto time = std::filesystem::last_write_time(file);
        std::filesystem::last_write_time(file, time + 2h);
        return time;
    }
};

#endif
