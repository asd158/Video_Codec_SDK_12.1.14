//*********************************************
// Created by weiya.gao 2020
//*********************************************


#pragma once

class HiObject
{
protected:
    bool _initFlag{false};
    virtual void OnInitAction();
    virtual void OnUninitAction();
public:
    HiObject() = default;
    virtual ~HiObject() = default;
    bool IsInitOk() const;
    virtual bool Init();
    virtual void Uninit();
};
