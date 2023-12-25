//*********************************************
// Created by weiya.gao 2020
//*********************************************

#include "HiObject.h"

bool HiObject::IsInitOk() const
{
    return _initFlag;
}

bool HiObject::Init()
{
    if (_initFlag)
    {
        return true;
    }
    OnInitAction();
    _initFlag = true;
    return true;
}

void HiObject::Uninit()
{
    if (!_initFlag)
    {
        return;
    }
    OnUninitAction();
    _initFlag = false;
}

void HiObject::OnInitAction()
{}

void HiObject::OnUninitAction()
{}
