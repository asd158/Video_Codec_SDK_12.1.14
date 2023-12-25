#include "inc/hvidrec.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <cuda.h>
#include "NvEncoderCuda.h"
#include "nvEncodeAPI.h"
#include "Logger.h"
#include <algorithm>
#include <HiUtils.h>

simplelogger::Logger *logger = simplelogger::LoggerFactory::CreateConsoleLogger();

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
    int baseWidth{-1};
    int baseHeight{-1};
    int baseSampleRate{-1};
    CUcontext baseCudaCtx{nullptr};
    NV_ENC_BUFFER_FORMAT baseBuffFormat{NV_ENC_BUFFER_FORMAT_IYUV};
    NvEncoderCuda *basePtrCudaEnc{nullptr};
    int baseFrameCnt{0};
    uint8_t *basePtrHostFrameCache{nullptr};
    std::vector<std::vector<uint8_t>> baseVPacketCache;
    std::string baseUuid;
    HiFile::FileWriter baseVidWriter;
    HiFile::FileWriter baseAudWriter;

    virtual ~VidInfo() {
        if (basePtrCudaEnc) {
            basePtrCudaEnc->DestroyEncoder();
            basePtrCudaEnc = nullptr;
        }
        if (baseCudaCtx) {
            cuCtxDestroy_v2(baseCudaCtx);
            baseCudaCtx = nullptr;
        }
        if (basePtrHostFrameCache) {
            delete basePtrHostFrameCache;
            basePtrHostFrameCache = nullptr;
        }
        baseVidWriter.Close();
    }
};

template<int level>
struct LevelVidInfo : public VidInfo {
    using PresetType = typename std::conditional
            <level == 0, LowVidPreset,
                    typename std::conditional
                            <level == 1, MidVidPreset, typename std::conditional
                                    <level == 2, HighVidPreset, void>::type>::type>::type;
    PresetType Preset{};

    static VidInfo *CreateVid(CUcontext cuc, int v_width, int v_height, int framerate, const std::string &export_dir) {
        auto vidInfo = new LevelVidInfo<level>();
        vidInfo->baseBuffFormat = NV_ENC_BUFFER_FORMAT_IYUV;
        vidInfo->baseWidth = v_width;
        vidInfo->baseHeight = v_height;
        vidInfo->baseSampleRate = framerate;
        vidInfo->baseCudaCtx = cuc;
        vidInfo->basePtrCudaEnc = new NvEncoderCuda(vidInfo->baseCudaCtx, vidInfo->baseWidth, vidInfo->baseHeight,
                                                    vidInfo->baseBuffFormat);
        vidInfo->baseFrameCnt = 0;
        vidInfo->basePtrHostFrameCache = new uint8_t[vidInfo->basePtrCudaEnc->GetFrameSize()];
        HiFile::mk_dir(export_dir);
        vidInfo->baseUuid = HiUtils::uuid();
        vidInfo->baseVidWriter.Open(HiFile::path_concat(export_dir, vidInfo->baseUuid + ".v"));
        InitializeEncoder(vidInfo);
        return vidInfo;
    };
};

template<class Vid>
void InitializeEncoder(Vid &&vid) {
    NV_ENC_INITIALIZE_PARAMS initializeParams = {NV_ENC_INITIALIZE_PARAMS_VER};
    NV_ENC_CONFIG encodeConfig = {NV_ENC_CONFIG_VER};
    initializeParams.encodeConfig = &encodeConfig;
    vid->basePtrCudaEnc->CreateDefaultEncoderParams(&initializeParams,
                                                    vid->Preset.ENC_CODEC,
                                                    vid->Preset.VPRESET,
                                                    vid->Preset.VTUNINGINFO);
    initializeParams.frameRateNum = vid->baseSampleRate;
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
    encodeConfig.rcParams.rateControlMode = vid->Preset.rateControlMode;
    encodeConfig.profileGUID = vid->Preset.profileGUID;
    vid->basePtrCudaEnc->CreateEncoder(&initializeParams);
}

void *hvid_record_open(int work_gpu, int v_width, int v_height, int framerate, int level, const char *export_dir) {
    ck(cuInit(0));
    int nGpu = 0;
    ck(cuDeviceGetCount(&nGpu));
    if (work_gpu < 0 || work_gpu >= nGpu) {
        return nullptr;
    }
    CUdevice cuDevice = 0;
    ck(cuDeviceGet(&cuDevice, work_gpu));
    char szDeviceName[80];
    ck(cuDeviceGetName(szDeviceName, sizeof(szDeviceName), cuDevice));
    std::cout << "GPU in use: " << szDeviceName << std::endl;
    CUcontext cuContext = nullptr;
    ck(cuCtxCreate(&cuContext, 0, cuDevice));
    if (level <= 0) {
        return LevelVidInfo<0>::CreateVid(cuContext, v_width, v_height, framerate, export_dir);
    } else if (level == 1) {
        return LevelVidInfo<1>::CreateVid(cuContext, v_width, v_height, framerate, export_dir);
    } else if (level == 2) {
        return LevelVidInfo<2>::CreateVid(cuContext, v_width, v_height, framerate, export_dir);
    } else {
        cuCtxDestroy_v2(cuContext);
        return nullptr;
    }
}

int hvid_record_get_vid_frame_buffsize(void *inst_id) {
    if (inst_id == nullptr) {
        return -1;
    }
    auto ptrVid = (VidInfo *) inst_id;
    return ptrVid->basePtrCudaEnc->GetFrameSize();
}

int hvid_record_get_vid_frame_count(void *inst_id) {
    if (inst_id == nullptr) {
        return -1;
    }
    auto ptrVid = (VidInfo *) inst_id;
    return ptrVid->baseFrameCnt;
}


int hvid_record_write_vid(void *inst_id, const char *vid_buff, int vid_buff_size, int is_final) {
    if (inst_id == nullptr) {
        return -1;
    }
    auto ptrVid = (VidInfo *) inst_id;
    int nFrameSize = ptrVid->basePtrCudaEnc->GetFrameSize();
    if (vid_buff_size != nFrameSize) {
        if (is_final) {
            ptrVid->baseVidWriter.Close();
            return 0;
        }
        return -2;
    }
    if ((bool) is_final) {
        const NvEncInputFrame *encoderInputFrame = ptrVid->basePtrCudaEnc->GetNextInputFrame();
        NvEncoderCuda::CopyToDeviceFrame(ptrVid->baseCudaCtx, ptrVid->basePtrHostFrameCache, 0,
                                         (CUdeviceptr) encoderInputFrame->inputPtr,
                                         (int) encoderInputFrame->pitch,
                                         ptrVid->basePtrCudaEnc->GetEncodeWidth(),
                                         ptrVid->basePtrCudaEnc->GetEncodeHeight(),
                                         CU_MEMORYTYPE_HOST,
                                         encoderInputFrame->bufferFormat,
                                         encoderInputFrame->chromaOffsets,
                                         encoderInputFrame->numChromaPlanes);
        ptrVid->basePtrCudaEnc->EncodeFrame(ptrVid->baseVPacketCache);
    } else {
        ptrVid->basePtrCudaEnc->EndEncode(ptrVid->baseVPacketCache);
    }
    ptrVid->baseFrameCnt += (int) ptrVid->baseVPacketCache.size();
    for (std::vector<uint8_t> &packet: ptrVid->baseVPacketCache) {
        // For each encoded packet
        ptrVid->baseVidWriter.Write(reinterpret_cast<char *>(packet.data()), packet.size());
    }
    if ((bool) is_final) {
        ptrVid->baseVidWriter.Close();
    }
    return 0;
}

int hvid_record_write_aud(void *inst_id) {
    return 0;
}

int hvid_record_close(void *inst_id) {
    if (inst_id == nullptr) {
        return -1;
    }
    auto ptrVid = (VidInfo *) inst_id;
    delete ptrVid;
    return 0;
}