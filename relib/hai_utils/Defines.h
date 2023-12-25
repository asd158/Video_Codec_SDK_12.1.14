//*********************************************
// Created by weiya.gao
// Copyright (c) 2020
//*********************************************

#pragma once

#include "log.h"

#ifndef M_PI
#define M_PI  3.14159265358979323846f
#endif
const char       WINDOWS_SLASH{'\\'};
const char       GOOD_SLASH{'/'};
const char       MOTION_EXT[]{".bytes"};
const char       ASSET_VERFILE[]{"ver"};
const char       HTTP_CACHE_ROOT[]{"__dlcache__"};
const char       UNZIP_CACHE_ROOT[]{"__uzcache__"};
const int        TTS_SAMPLE_RATE     = 16000;
const double     EXP_M1{0.36787944117L};
const int        WAV_BUFF_MAXSIZE    = TTS_SAMPLE_RATE * 30 * 2;
const char       SCENE_NODE_PRE_ID[]{ "SCENENODE_" }; //scene node ID
typedef struct alignas(4) WAV_HEADER {
    uint8_t  RIFF[4];        // RIFF Header Magic header
    uint32_t ChunkSize;      // RIFF Chunk Size
    uint8_t  WAVE[4];        // WAVE Header
    uint8_t  fmt[4];         // FMT header
    uint32_t Subchunk1Size;  // Size of the fmt chunk
    uint16_t AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw,     257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    uint16_t NumOfChan;      // Number of channels 1=Mono 2=Sterio
    uint32_t SamplesPerSec;  // Sampling Frequency in Hz
    uint32_t bytesPerSec;    // bytes per second
    uint16_t blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    uint16_t bitsPerSample;  // Number of bits per sample
    /* "data" sub-chunk */
    uint8_t  Subchunk2ID[4]; // "data"  string
    uint32_t Subchunk2Size;  // Sampled data length
}                WAV_HEADER;
const static int WAV_HEADER_SIZE     = 44;
const double     PROJECT_WORLDUNIT   = 5;
const float      PROJ_NEAR           = -1000;
const float      PROJ_FAR            = 1000;
const float      EYE_Z               = 100;
const float      LAYER_Z_FX          = EYE_Z;
const float      LAYER_Z_BLEND_INOUT = -((PROJ_FAR - EYE_Z) - (PROJ_FAR - PROJ_NEAR) * 40 / 200.0);
const float      LAYER_Z_BACKGROUND  = -((PROJ_FAR - EYE_Z) - (PROJ_FAR - PROJ_NEAR) * 1 / 200.0);
const float      LAYER_Z_COLOR       = -((PROJ_FAR - EYE_Z) - (PROJ_FAR - PROJ_NEAR) * 2 / 200.0);
const float      LAYER_Z_CUBISM      = 500;

