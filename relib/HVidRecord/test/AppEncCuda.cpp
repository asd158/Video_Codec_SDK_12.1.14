#include <fstream>
#include <iostream>
#include <memory>
#include <cuda.h>
#include "NvEncoder/NvEncoderCuda.h"

simplelogger::Logger *logger = simplelogger::LoggerFactory::CreateConsoleLogger();

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

void EncodeCuda(int nWidth, int nHeight,
                int targetFrameRate,
                GUID encode, GUID preset, NV_ENC_TUNING_INFO tuning_info, NV_ENC_BUFFER_FORMAT eFormat,
                CUcontext cuContext, std::ifstream &fpIn, std::ofstream &fpOut) {
    std::unique_ptr<NvEncoderCuda> pEnc(new NvEncoderCuda(cuContext, nWidth, nHeight, eFormat));
    InitializeEncoder(pEnc, encode, preset, tuning_info, targetFrameRate);

    int nFrameSize = pEnc->GetFrameSize();

    std::unique_ptr<uint8_t[]> pHostFrame(new uint8_t[nFrameSize]);
    int nFrame = 0;
    while (true) {
        // Load the next frame from disk
        std::streamsize nRead = fpIn.read(reinterpret_cast<char *>(pHostFrame.get()), nFrameSize).gcount();
        // For receiving encoded packets
        std::vector<std::vector<uint8_t>> vPacket;
        if (nRead == nFrameSize) {
            const NvEncInputFrame *encoderInputFrame = pEnc->GetNextInputFrame();
            NvEncoderCuda::CopyToDeviceFrame(cuContext, pHostFrame.get(), 0, (CUdeviceptr) encoderInputFrame->inputPtr,
                                             (int) encoderInputFrame->pitch,
                                             pEnc->GetEncodeWidth(),
                                             pEnc->GetEncodeHeight(),
                                             CU_MEMORYTYPE_HOST,
                                             encoderInputFrame->bufferFormat,
                                             encoderInputFrame->chromaOffsets,
                                             encoderInputFrame->numChromaPlanes);

            pEnc->EncodeFrame(vPacket);
        } else {
            pEnc->EndEncode(vPacket);
        }
        nFrame += (int) vPacket.size();
        for (std::vector<uint8_t> &packet: vPacket) {
            // For each encoded packet
            fpOut.write(reinterpret_cast<char *>(packet.data()), packet.size());
        }

        if (nRead != nFrameSize) break;
    }

    pEnc->DestroyEncoder();

    std::cout << "Total frames encoded: " << nFrame << std::endl;
}

int main() {
    char szInFilePath[256] = "F:\\code\\Video_Codec_SDK_12.1.14\\test\\aa.yuv";

    char szOutFilePath[256] = "F:\\code\\Video_Codec_SDK_12.1.14\\test\\aa_yuv.hevc";
    NV_ENC_BUFFER_FORMAT EFORMAT = NV_ENC_BUFFER_FORMAT_IYUV;
    int nWidth = 1280;
    int nHeight = 720;
    int frameRate = 25;
//    GUID ENC_CODEC = NV_ENC_CODEC_H264_GUID;
//    GUID VPRESET = NV_ENC_PRESET_P7_GUID;
//    NV_ENC_TUNING_INFO VTUNINGINFO = NV_ENC_TUNING_INFO_HIGH_QUALITY;
    GUID ENC_CODEC = NV_ENC_CODEC_HEVC_GUID;
    GUID VPRESET = NV_ENC_PRESET_P7_GUID;
    NV_ENC_TUNING_INFO VTUNINGINFO = NV_ENC_TUNING_INFO_HIGH_QUALITY;
/////////////////////////////////////////////////////////
//                NV_ENC_CODEC_H264_GUID,
//                NV_ENC_CODEC_HEVC_GUID,
//                NV_ENC_CODEC_AV1_GUID
/////////////////////////////////////////////////////////
//                NV_ENC_PRESET_P1_GUID,
//                NV_ENC_PRESET_P2_GUID,
//                NV_ENC_PRESET_P3_GUID,
//                NV_ENC_PRESET_P4_GUID,
//                NV_ENC_PRESET_P5_GUID,
//                NV_ENC_PRESET_P6_GUID,
//                NV_ENC_PRESET_P7_GUID,
/////////////////////////////////////////////////////////
//                NV_ENC_TUNING_INFO_HIGH_QUALITY,
//                NV_ENC_TUNING_INFO_LOW_LATENCY,
//                NV_ENC_TUNING_INFO_ULTRA_LOW_LATENCY,
//                NV_ENC_TUNING_INFO_LOSSLESS
/////////////////////////////////////////////////////////
//                NV_ENC_BUFFER_FORMAT_IYUV,
//                NV_ENC_BUFFER_FORMAT_NV12,
//                NV_ENC_BUFFER_FORMAT_YV12,
//                NV_ENC_BUFFER_FORMAT_YUV444,
//                NV_ENC_BUFFER_FORMAT_YUV420_10BIT,
//                NV_ENC_BUFFER_FORMAT_YUV444_10BIT,
//                NV_ENC_BUFFER_FORMAT_ARGB,
//                NV_ENC_BUFFER_FORMAT_ARGB10,
//                NV_ENC_BUFFER_FORMAT_AYUV,
//                NV_ENC_BUFFER_FORMAT_ABGR,
//                NV_ENC_BUFFER_FORMAT_ABGR10,
    try {
        ck(cuInit(0));
        int nGpu = 0;
        ck(cuDeviceGetCount(&nGpu));
        int iGpu = 0;
        if (iGpu < 0 || iGpu >= nGpu) {
            std::cout << "GPU ordinal out of range. Should be within [" << 0 << ", " << nGpu - 1 << "]" << std::endl;
            return 1;
        }
        CUdevice cuDevice = 0;
        ck(cuDeviceGet(&cuDevice, iGpu));
        char szDeviceName[80];
        ck(cuDeviceGetName(szDeviceName, sizeof(szDeviceName), cuDevice));
        std::cout << "GPU in use: " << szDeviceName << std::endl;
        CUcontext cuContext = NULL;
        ck(cuCtxCreate(&cuContext, 0, cuDevice));

        // Open input file
        std::ifstream fpIn(szInFilePath, std::ifstream::in | std::ifstream::binary);
        if (!fpIn) {
            std::ostringstream err;
            err << "Unable to open input file: " << szInFilePath << std::endl;
            throw std::invalid_argument(err.str());
        }

        // Open output file
        std::ofstream fpOut(szOutFilePath, std::ios::out | std::ios::binary);
        if (!fpOut) {
            std::ostringstream err;
            err << "Unable to open output file: " << szOutFilePath << std::endl;
            throw std::invalid_argument(err.str());
        }

        // Encode
        EncodeCuda(nWidth, nHeight, frameRate,
                   ENC_CODEC, VPRESET, VTUNINGINFO, EFORMAT,
                   cuContext, fpIn, fpOut);

        fpOut.close();
        fpIn.close();

        std::cout << "Bitstream saved in file " << szOutFilePath << std::endl;
    }
    catch (const std::exception &ex) {
        std::cout << ex.what();
        return 1;
    }
    return 0;
}
