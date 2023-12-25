//*********************************************
// Created by weiya.gao 2020
//*********************************************

#include <algorithm>
#include "RootPath.h"
#include "HiUtils.h"

static std::string gPersistenceRoot{};

void RootPath::SetPersistenceRoot(const std::string &persistent) {
    std::string str = persistent;
    std::replace(str.begin(),
                 str.end(),
                 WINDOWS_SLASH,
                 GOOD_SLASH);
    if (str.empty()) {
        str = ".";
    }
    const auto finalPath = str[str.size() - 1];
    if (finalPath == GOOD_SLASH) {
        gPersistenceRoot = str.substr(0,
                                      str.size() - 1);
    }
    else {
        gPersistenceRoot = str;
    }
    HiFile::mk_dir(gPersistenceRoot);
}

const std::string &RootPath::GetPersistenceRoot() {
    return gPersistenceRoot;
}
