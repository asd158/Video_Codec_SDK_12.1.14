//*********************************************
// Created by weiya.gao 2020
//*********************************************

#pragma once
#include <string>
namespace RootPath
{
    void SetPersistenceRoot(const std::string &persistent);
    const std::string &GetPersistenceRoot();
}
