#include <iostream>
#include <fmt/ranges.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>
//https://www.youtube.com/watch?v=NcEE5xmpgQ0

auto toBinary(int num) -> std::string
{
    //auto vector = std::vector<int>();
    auto s = std::string();
    if(num == 0)
    {
        return "00000000";
    }
    while(num > 0)
    {
        s+= static_cast<char>(num%2);
        num /= 2;
        fmt::println("{}",s);
    }

    std::ranges::reverse(s);
    return s;
}

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

struct Colors
{
    int R = 0;
    int G= 0;
    int B= 0;
};

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
    int fileSize;
    std::vector<std::vector<unsigned char>> pixelsArray;
    unsigned long long messLen;
    int size;

public:
    auto readFile(std::string const &path) {
        auto file = std::fstream(path, std::ios::in | std::ios::binary);

        if (!file.is_open()) {
            fmt::println("Cannot open a image");
            exit(EXIT_FAILURE);
        }

        //zrozumiec programowanie - Gynvael Coldwind
        file.read((char *) &fh, sizeof(FILEHEADER));
        file.read((char *) &fih, sizeof(FILEINFOHEADER));
        fmt::println("{}", fh.bfSize);
        fmt::println("{}", fih.biBitCount);

        if (fh.bfType[0] != 'B' && fh.bfType[1] != 'M') {
            fmt::println("The file is not BMP format");
            exit(EXIT_FAILURE);
        }

        fmt::println("{} {}", fih.biWidth, fih.biHeight);

        for (int i = 0; i < fih.biHeight; ++i) {
            for (int j = 0; j < fih.biWidth; ++j) {
                auto pixels = std::vector<unsigned char>{0, 0, 0};
                file.read(reinterpret_cast<char *>(pixels.data()), 3);
                pixelsArray.push_back(pixels);
//                pixelsArray[j * biWidth + i].R = (int)pixel[2];
//                pixelsArray[j * biWidth + i].G = (int)(pixel[1]);
//                pixelsArray[j * biWidth + i].B = (int)(pixel[0]);
//                fmt::println("{}", pixelsArray[j * biWidth + i].R);
            }
        }
        size = sizeof(FILEHEADER) + sizeof(FILEINFOHEADER) + (pixelsArray.size() * 3);
        fmt::println("{}", size);
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

    auto encode(std::string& txt) {
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

    auto getFileSize() -> int {
        return fileSize;
    }

    auto getMessLen() -> unsigned long long
    {
        return messLen;
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

};


auto checkExt(std::string path) -> bool
{
    auto end = path.end()-4;
    if(path.substr(path.find(*end),4) == ".bmp")
    {
        return true;
    }
    else if(path.substr(path.find(*end),4) == ".jpg")
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
    auto path = std::string("C:\\Users\\Krystian\\Desktop\\Studia\\pjc\\steganography_pjc\\testpng.png");
    auto bmp = BMP();
//    bmp.readFile(path);
//    bmp.encode("basia");
//    bmp.writeFile();
//    auto path = std::string("C:\\Users\\Krystian\\Desktop\\Studia\\pjc\\steganography_pjc\\cmake-build-debug\\newFile.bmp");
//    bmp.readFile(path);
//    fmt::println("{}", bmp.decode());
    auto png = PNG();
    png.readFile(path);
//    auto c = Colors();
//    c.R = 2;
//    fmt::println("{}",c.R);
//    fmt::println("{}",argv[1]);
//    if(strcmp(argv[1],"-i") == 0 || strcmp(argv[1], "-info") == 0)
//    {
//        if(checkExt(path)) {
//            readFile(path);
//        }
//        else
//        {
//            fmt::println("Given format is not supported");
//        }
//
//    }
//    else if (strcmp(argv[1], "-e") == 0 || strcmp(argv[1], "-encrypt") == 0)
//    {
//        if(checkExt(path)) {
//            readFile(path);
//        }
//        else
//        {
//            fmt::println("Given format is not supported");
//        }
//    }
//    else if (strcmp(argv[1], "-d") == 0|| strcmp(argv[1], "-decrypt") == 0)
//    {
//        if(checkExt(path)) {
//            readFile(path);
//        }
//        else
//        {
//            fmt::println("Given format is not supported");
//        }
//    }
//    else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0)
//    {
//        if(checkExt(path)) {
//            readFile(path);
//        }
//        else
//        {
//            fmt::println("Given format is not supported");
//        }
//    }
}
