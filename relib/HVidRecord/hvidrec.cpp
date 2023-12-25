#include "inc/hvidrec.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <cuda.h>
#include "NvEncoderCuda.h"
#include "nvEncodeAPI.h"
#include  <algorithm>

struct HighVidPreset {
    GUID ENC_CODEC = NV_ENC_CODEC_HEVC_GUID;
    GUID VPRESET = NV_ENC_PRESET_P7_GUID;
    NV_ENC_TUNING_INFO VTUNINGINFO = NV_ENC_TUNING_INFO_HIGH_QUALITY;
    NV_ENC_PARAMS_RC_MODE rateControlMode = NV_ENC_PARAMS_RC_CONSTQP;
    GUID profileGUID = NV_ENC_HEVC_PROFILE_MAIN_GUID;
};
struct MidVidPreset {
    GUID ENC_CODEC = NV_ENC_CODEC_HEVC_GUID;
    GUID VPRESET = NV_ENC_PRESET_P7_GUID;
    NV_ENC_TUNING_INFO VTUNINGINFO = NV_ENC_TUNING_INFO_HIGH_QUALITY;
    NV_ENC_PARAMS_RC_MODE rateControlMode = NV_ENC_PARAMS_RC_CONSTQP;
    GUID profileGUID = NV_ENC_HEVC_PROFILE_MAIN_GUID;
};
struct LowVidPreset {
    GUID ENC_CODEC = NV_ENC_CODEC_HEVC_GUID;
    GUID VPRESET = NV_ENC_PRESET_P7_GUID;
    NV_ENC_TUNING_INFO VTUNINGINFO = NV_ENC_TUNING_INFO_HIGH_QUALITY;
    NV_ENC_PARAMS_RC_MODE rateControlMode = NV_ENC_PARAMS_RC_CONSTQP;
    GUID profileGUID = NV_ENC_HEVC_PROFILE_MAIN_GUID;
};

struct VidInfo {
    int nWidth{-1};
    int nHeight{-1};
    CUcontext cuc{nullptr};
    NV_ENC_BUFFER_FORMAT EFORMAT{NV_ENC_BUFFER_FORMAT_IYUV};
    NvEncoderCuda *pEnc{nullptr};
    int nFrame{0};
    uint8_t *pHostFrame{nullptr};
    std::vector<std::vector<uint8_t>> vPacket;

    virtual ~VidInfo() {
        if (pEnc) {
            pEnc->DestroyEncoder();
            pEnc = nullptr;
        }
        if (cuc) {
            cuCtxDestroy_v2(cuc);
            cuc = nullptr;
        }
        if (pHostFrame) {
            delete pHostFrame;
            pHostFrame = nullptr;
        }
    }
};

template<int level>
struct LevelVidInfo : public VidInfo {
    using PresetType = typename std::conditional
            <level == 0, LowVidPreset,
                    typename std::conditional
                            <level == 1, MidVidPreset, typename std::conditional
                                    <level == 2, HighVidPreset, void>::type>::type>::type;
    PresetType preset{};

    static VidInfo *CreateVid(CUcontext cuc, int v_width, int v_height, int framerate) {
        auto vidInfo = new LevelVidInfo<level>();
        vidInfo->EFORMAT = NV_ENC_BUFFER_FORMAT_IYUV;
        vidInfo->nWidth = v_width;
        vidInfo->nHeight = v_height;
        vidInfo->cuc = cuc;
        vidInfo->pEnc = new NvEncoderCuda(vidInfo->cuc, vidInfo->nWidth, vidInfo->nHeight, vidInfo->EFORMAT);
        vidInfo->nFrame = 0;
        vidInfo->pHostFrame = new uint8_t[vidInfo->pEnc->GetFrameSize()];
        InitializeEncoder(vidInfo->pEnc, vidInfo->preset.ENC_CODEC, vidInfo->preset.VPRESET,
                          vidInfo->preset.VTUNINGINFO,
                          framerate);
        return vidInfo;
    };
};

template<class EncoderClass>
void
InitializeEncoder(EncoderClass &pEnc, GUID encode, GUID preset, NV_ENC_TUNING_INFO tuning_info, int targetFrameRate) {
    NV_ENC_INITIALIZE_PARAMS initializeParams = {NV_ENC_INITIALIZE_PARAMS_VER};
    NV_ENC_CONFIG encodeConfig = {NV_ENC_CONFIG_VER};
    initializeParams.encodeConfig = &encodeConfig;
    pEnc->CreateDefaultEncoderParams(&initializeParams, encode, preset, tuning_info);
    initializeParams.frameRateNum = targetFrameRate;
    initializeParams.frameRateDen = 1;
//    encodeConfig.rcParams.averageBitRate
//    encodeConfig.rcParams.maxBitRate
//    encodeConfig.rcParams.vbvBufferSize
//    encodeConfig.rcParams.vbvInitialDelay
//    encodeConfig.rcParams.targetQuality
//    encodeConfig.rcParams.initialRCQP

//    encodeConfig.rcParams.enableMinQP
//    encodeConfig.rcParams.minQP
//    encodeConfig.rcParams.enableMaxQP
//    encodeConfig.rcParams.maxQP
//    encodeConfig.rcParams.constQP

//    encodeConfig.rcParams.enableAQ
//    encodeConfig.rcParams.aqStrength

//    encodeConfig.rcParams.enableLookahead
//    encodeConfig.rcParams.lookaheadDepth
    encodeConfig.rcParams.multiPass = NV_ENC_MULTI_PASS_DISABLED;
//    NV_ENC_PARAMS_RC_CONSTQP,
//    NV_ENC_PARAMS_RC_VBR,
//    NV_ENC_PARAMS_RC_CBR,
    encodeConfig.rcParams.rateControlMode = NV_ENC_PARAMS_RC_CONSTQP;
    if (NV_ENC_CODEC_H264_GUID == encode) {
//        NV_ENC_H264_PROFILE_BASELINE_GUID,
//        NV_ENC_H264_PROFILE_MAIN_GUID,
//        NV_ENC_H264_PROFILE_HIGH_GUID,
//        NV_ENC_H264_PROFILE_HIGH_444_GUID,
        encodeConfig.profileGUID = NV_ENC_H264_PROFILE_HIGH_GUID;
//        encodeConfig.encodeCodecConfig.h264Config.idrPeriod = config.gopLength;
//        YUV444 Input
//        encodeConfig.encodeCodecConfig.h264Config.chromaFormatIDC = 3;
    } else if (NV_ENC_CODEC_HEVC_GUID == encode) {
//        NV_ENC_HEVC_PROFILE_MAIN_GUID,
//        NV_ENC_HEVC_PROFILE_MAIN10_GUID,
//        NV_ENC_HEVC_PROFILE_FREXT_GUID,
        encodeConfig.profileGUID = NV_ENC_HEVC_PROFILE_MAIN_GUID;
//        YUV444 Input
//        encodeConfig.encodeCodecConfig.hevcConfig.chromaFormatIDC = 3;

    } else {
//        NV_ENC_AV1_PROFILE_MAIN_GUID,
        encodeConfig.profileGUID = NV_ENC_AV1_PROFILE_MAIN_GUID;
//        encodeConfig.encodeCodecConfig.av1Config.idrPeriod = config.gopLength;
    }
    pEnc->CreateEncoder(&initializeParams);
}

void *hvid_record_open(int v_width, int v_height, int framerate, int level) {
    ck(cuInit(0));
    int nGpu = 0;
    ck(cuDeviceGetCount(&nGpu));
    int iGpu = 0;
    if (iGpu < 0 || iGpu >= nGpu) {
        return nullptr;
    }
    CUdevice cuDevice = 0;
    ck(cuDeviceGet(&cuDevice, iGpu));
    char szDeviceName[80];
    ck(cuDeviceGetName(szDeviceName, sizeof(szDeviceName), cuDevice));
    std::cout << "GPU in use: " << szDeviceName << std::endl;
    CUcontext cuContext = nullptr;
    ck(cuCtxCreate(&cuContext, 0, cuDevice));
    if (level <= 0) {
        return LevelVidInfo<0>::CreateVid(cuContext, v_width, v_height, framerate);
    } else if (level == 1) {
        return LevelVidInfo<1>::CreateVid(cuContext, v_width, v_height, framerate);
    } else if (level == 2) {
        return LevelVidInfo<2>::CreateVid(cuContext, v_width, v_height, framerate);
    } else {
        cuCtxDestroy_v2(cuContext);
        return nullptr;
    }
}

int hvid_record_write_vid(void *inst_id, const char *vid_buff, int vid_buff_size, int is_final) {
    if (inst_id == nullptr) {
        return -1;
    }
    auto ptrVid = (VidInfo *) inst_id;
    int nFrameSize = ptrVid->pEnc->GetFrameSize();
    if (vid_buff_size != nFrameSize) {
        return -2;
    }
    if ((bool) is_final) {
        const NvEncInputFrame *encoderInputFrame = ptrVid->pEnc->GetNextInputFrame();
        NvEncoderCuda::CopyToDeviceFrame(ptrVid->cuc, ptrVid->pHostFrame, 0, (CUdeviceptr) encoderInputFrame->inputPtr,
                                         (int) encoderInputFrame->pitch,
                                         ptrVid->pEnc->GetEncodeWidth(),
                                         ptrVid->pEnc->GetEncodeHeight(),
                                         CU_MEMORYTYPE_HOST,
                                         encoderInputFrame->bufferFormat,
                                         encoderInputFrame->chromaOffsets,
                                         encoderInputFrame->numChromaPlanes);
        ptrVid->pEnc->EncodeFrame(ptrVid->vPacket);
    } else {
        ptrVid->pEnc->EndEncode(ptrVid->vPacket);
    }
    ptrVid->nFrame += (int) ptrVid->vPacket.size();

    for (std::vector<uint8_t> &packet: ptrVid->vPacket) {
        // For each encoded packet
        fpOut.write(reinterpret_cast<char *>(packet.data()), packet.size());
    }
    return 0;
}

int hvid_record_write_aud(void *inst_id) {
    return 0;
}

int hvid_record_close(void *inst_id, const char *export_dir) {
    if (inst_id == nullptr) {
        return -1;
    }
    auto ptrVid = (VidInfo *) inst_id;
    delete ptrVid;
    return 0;
}
