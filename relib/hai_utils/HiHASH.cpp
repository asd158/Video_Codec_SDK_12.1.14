//*********************************************
// Created by weiya.gao 2020
//*********************************************


#include "HiUtils.h"

HiHASH::StreamingHash::StreamingHash()
{
    _hdl = XXH32_createState();
    unsigned int const seed = 0;
    XXH32_reset(_hdl,
                seed);
}

void HiHASH::StreamingHash::AppendBuff(std::string_view buff)
{
    XXH32_update(_hdl,
                 buff.data(),
                 buff.size());
}

unsigned int HiHASH::StreamingHash::GetHash()
{
    return XXH32_digest(_hdl);
}

HiHASH::StreamingHash::~StreamingHash()
{
    XXH32_freeState((XXH32_state_t *) _hdl);
    _hdl = nullptr;
}