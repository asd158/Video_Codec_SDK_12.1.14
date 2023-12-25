//*********************************************
// Created by weiya.gao 2020
//*********************************************
#if defined(__EMSCRIPTEN__)
#   include <filesystem>
namespace fs = std::filesystem;
#else

#   include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;
#endif
#define fopen_s(pFile, filename, mode) ((*(pFile))=fopen((filename),(mode)))==NULL

#include "HiUtils.h"
#include <algorithm>
#include <fstream>
#include <random>

int HiFile::readbyte_from_file(const std::string &path,
                               char *inputBuff,
                               int inputBuffSize) {
    if (path.empty() || inputBuff == nullptr || inputBuffSize <= 0) {
        return -1;
    }
    FILE *readFile = nullptr;
    fopen_s(&readFile,
            path.c_str(),
            "rb");
    if (readFile == nullptr) {
        return -2;
    }
    if (ferror(readFile)) {
        return -3;
    }
    int    total = 0;
    size_t len;
    while (!feof(readFile)) {
        len = static_cast<int>(fread(inputBuff + total,
                                     1,
                                     1024,
                                     readFile));
        total += len;
    }
    fclose(readFile);
    return total;
}
bool HiFile::savebyte_to_file(const std::string &path,
                              const char *buff,
                              int buffSize) {
    if (buffSize <= 0) {
        return false;
    }
    FILE *ptrfile = nullptr;
    auto &&dir    = HiFile::get_dirpath(path);
    HiFile::mk_dir(dir);
    fopen_s(&ptrfile,
            path.c_str(),
            "wb");
    if (ptrfile == nullptr) {
        return false;
    }
    if (ferror(ptrfile)) {
        return false;
    }
    fwrite(buff,
           1,
           buffSize,
           ptrfile);
    fclose(ptrfile);
    return true;
}
bool HiFile::copy_file(const std::string &from,
                       const std::string &to) {
    if (!is_file_exist(from)) {
        return false;
    }
    auto &&dirPath = get_dirpath(to);
    if (!is_file_exist(dirPath)) {
        mk_dir(dirPath);
    }
    return fs::copy_file(from,
                         to,
                         fs::copy_options::overwrite_existing);
}
bool HiFile::mk_dir(const std::string &path) {
    if (path.empty()) {
        return true;
    }
    return fs::create_directories(path);
}
void HiFile::remove_all_files(const std::string &path) {
    if (path.empty()) {
        return;
    }
    if (fs::exists(path)) {
        if (fs::is_directory(path)) {
            fs::remove_all(path);
        }
        else {
            fs::remove(path);
        }
    }
}
std::string HiFile::get_filename(const std::string &path) {
    return fs::path(path).filename().string();
}
std::string HiFile::get_filename_without_ext(const std::string &path) {
    auto &&fileName = fs::path(path).filename();
    return fileName.replace_extension().string();
}
std::string HiFile::get_path_without_ext(const std::string &path) {
    auto &&fileName = fs::path(path);
    return fileName.replace_extension().string();
}
std::string HiFile::get_fileext(const std::string &path) {
    return fs::path(path).filename().extension().string();
}
std::string HiFile::get_dirpath(const std::string &path) {
    fs::path p1 = path;
    return p1.parent_path().string();
}
std::string HiFile::get_parent_dirname(const std::string &path) {
    fs::path p1 = path;
    return p1.parent_path().filename().string();
}
std::string HiFile::path_concat(const std::string &path,
                                const std::string &path1) {
    std::string p1 = path;
    std::string p2 = path1;
    if (!p1.empty()) {
        if (!(p1[p1.size() - 1] == GOOD_SLASH || p1[p1.size() - 1] == WINDOWS_SLASH)) {
            p1 += GOOD_SLASH;
        }
    }
    REV:
    if (!p2.empty()) {
        if (p2[0] == GOOD_SLASH || p2[0] == WINDOWS_SLASH) {
            p2 = p2.c_str() + 1;
            goto REV;
        }
        else {
            goto ENDL;
        }
    }
    ENDL:
    return p1 + p2;
}
bool HiFile::is_file_exist(const std::string &fileName) {
    if (is_dir(fileName)) {
        return false;
    }
    return fs::exists(fileName);
}
bool HiFile::is_dir(const std::string &path) {
    return fs::is_directory(path);
}
bool HiFile::FileWriter::Open(const std::string &filePath) {
    if (_file != nullptr) {
        return false;
    }
    auto &&dirPath = get_dirpath(filePath);
    if (!is_file_exist(dirPath)) {
        mk_dir(dirPath);
    }
    fopen_s(&_file,
            filePath.c_str(),
            "wb");
    return true;
}
HiFile::FileWriter::~FileWriter() {
    Close();
}
int HiFile::FileWriter::Write(const void *buffer,
                              size_t sizeToWrite) {
    if (_file == nullptr) {
        return -1;
    }
    size_t const writtenSize = fwrite(buffer,
                                      1,
                                      sizeToWrite,
                                      _file);
    if (writtenSize == sizeToWrite) {
        return sizeToWrite;
    }
    return 0;
}
void HiFile::FileWriter::Close() {
    if (_file == nullptr) {
        return;
    }
    fclose(_file);
    _file = nullptr;
}

bool HiFile::FileWriter::IsOpened() {
    return _file!= nullptr;
}

bool HiFile::FileReader::Open(const std::string &filePath) {
    if (_file != nullptr) {
        return false;
    }
    if (!is_file_exist(filePath)) {
        return false;
    }
    fopen_s(&_file,
            filePath.c_str(),
            "rb");
    return true;
}
HiFile::FileReader::~FileReader() {
    Close();
}
int HiFile::FileReader::Read(void *buffer,
                             size_t sizeToRead) {
    if (_file == nullptr) {
        return -1;
    }
    size_t const readSize = fread(buffer,
                                  1,
                                  sizeToRead,
                                  _file);
    if (readSize == sizeToRead) {
        return readSize;
    }
    if (feof(_file)) {
        return readSize;
    }
    return 0;
}
void HiFile::FileReader::Close() {
    if (_file == nullptr) {
        return;
    }
    fclose(_file);
    _file = nullptr;
}
HiFile::FileAppend::FileAppend(const std::string &filePath) {
    auto &&dirPath = get_dirpath(filePath);
    if (!is_file_exist(dirPath)) {
        mk_dir(dirPath);
    }
    fopen_s(&_file,
            filePath.c_str(),
            "ab");
}
HiFile::FileAppend::~FileAppend() {
    if (_file == nullptr) {
        return;
    }
    fclose(_file);
    _file = nullptr;
}
int HiFile::FileAppend::Append(void *buffer,
                               size_t appendSize) {
    if (_file == nullptr) {
        return -1;
    }
    size_t const writtenSize = fwrite(buffer,
                                      1,
                                      appendSize,
                                      _file);
    if (writtenSize == appendSize) {
        return appendSize;
    }
    return -2;
}
size_t HiFile::file_size(const std::string &filename) {
    if (fs::exists(filename)) {
        return fs::file_size(filename);
    }
    return 0;
}
std::vector<std::string> HiFile::enum_dir(const std::string &dirPath,
                                          bool onlyDir,
                                          bool recursive) {
    std::vector<std::string> retList{};
    if (!fs::exists(dirPath)) {
        return retList;
    }
    if (recursive) {
        auto      begin = fs::recursive_directory_iterator(dirPath);
        auto      end   = fs::recursive_directory_iterator();
        for (auto it    = begin;
             it != end;
             it++) {
            auto &entry = *it;
            if (onlyDir) {
                if (fs::is_directory(entry)) {
                    std::string path = entry.path().string().c_str();
                    std::replace(path.begin(),
                                 path.end(),
                                 WINDOWS_SLASH,
                                 GOOD_SLASH);
                    retList.emplace_back(path);
                }
            }
            else {
                if (fs::is_regular_file(entry)) {
                    std::string path = entry.path().string().c_str();
                    std::replace(path.begin(),
                                 path.end(),
                                 WINDOWS_SLASH,
                                 GOOD_SLASH);
                    retList.emplace_back(path);
                }
            }
        }
    }
    else {
        auto      begin = fs::directory_iterator(dirPath);
        auto      end   = fs::directory_iterator();
        for (auto it    = begin;
             it != end;
             it++) {
            auto &entry = *it;
            if (onlyDir) {
                if (fs::is_directory(entry)) {
                    std::string path = entry.path().string().c_str();
                    std::replace(path.begin(),
                                 path.end(),
                                 WINDOWS_SLASH,
                                 GOOD_SLASH);
                    retList.emplace_back(path);
                }
            }
            else {
                if (fs::is_regular_file(entry)) {
                    std::string path = entry.path().string().c_str();
                    std::replace(path.begin(),
                                 path.end(),
                                 WINDOWS_SLASH,
                                 GOOD_SLASH);
                    retList.emplace_back(path);
                }
            }
        }
    }
    return retList;
}