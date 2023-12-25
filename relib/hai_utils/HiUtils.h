#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <string_view>
#include <rapidjson/document.h>
#include "log.h"
#include "xxh3.h"

namespace HiFile {
    int readbyte_from_file(const std::string &path, char *inputBuff, int inputBuffSize);
    bool savebyte_to_file(const std::string &path, const char *buff, int buffSize);
    bool copy_file(const std::string &from, const std::string &to);
    bool mk_dir(const std::string &path);
    void remove_all_files(const std::string &path);
    std::string get_filename(const std::string &path);
    std::string get_filename_without_ext(const std::string &path);
    std::string get_path_without_ext(const std::string &path);
    std::string get_fileext(const std::string &path);
    std::string get_dirpath(const std::string &path);
    std::string get_parent_dirname(const std::string &path);
    std::string path_concat(const std::string &path, const std::string &path1);
    bool is_file_exist(const std::string &fileName);
    bool is_dir(const std::string &path);

    class FileWriter {
    public:
        bool Open(const std::string &filePath);
        void Close();
        ~FileWriter();
        int Write(const void *buffer, size_t sizeToWrite);
    private:
        FILE *_file{nullptr};
    };

    class FileReader {
    public:
        bool Open(const std::string &filePath);
        void Close();
        ~FileReader();
        int Read(void *buffer, size_t sizeToRead);
    private:
        FILE *_file{nullptr};
    };

    class FileAppend {
    public:
        explicit FileAppend(const std::string &filePath);
        ~FileAppend();
        int Append(void *buffer, size_t appendSize);
    private:
        FILE *_file{nullptr};
    };

    size_t file_size(const std::string &filename);
    size_t append_buff_tofile(const std::string &filePath, std::string_view buff);
    std::vector<std::string> enum_dir(const std::string &dirPath, bool onlyDir, bool recursive);
}
namespace HiUtils {
    std::vector<std::string> split_str(const std::string &strv, char delim = ' ');
    std::vector<std::string_view> split_str(std::string_view strv, std::string_view delims = " ");
    int random_int(int minInt, int maxInt);
    std::string uuid();
    long now_seconds();
    std::string replace_all(std::string str, const std::string &from, const std::string &to);
    char toLowerCh(char c);
    void toLowerInPlace(std::string &s);
    std::string toLower(std::string const &s);
    std::vector<std::string> splitMatch(const std::string& matchName, const std::string& matchRegex);
}
namespace HiHASH {
    class StreamingHash {
    public:
        explicit StreamingHash();
        void AppendBuff(std::string_view buff);
        unsigned int GetHash();
        ~StreamingHash();
    private:
        XXH32_state_t *_hdl;
    };
}