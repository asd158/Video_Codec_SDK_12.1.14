IF (WIN32)
    SET(EXT_DIR x64)
ELSE ()
    IF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        SET(EXT_DIR linux/x86_64)
    ELSEIF (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
        SET(EXT_DIR linux/aarch64)
    ELSE ()
        MESSAGE(FATAL_ERROR not support ${CMAKE_SYSTEM_PROCESSOR})
    ENDIF ()
ENDIF ()
SET(NVCodecSDK_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/3rd/NVCodecSDK/inc)
SET(nvcuvid_LIBRARY ${CMAKE_SOURCE_DIR}/3rd/NVCodecSDK/lib/${EXT_DIR}/nvcuvid.a)
SET(nvencodeapi_LIBRARY ${CMAKE_SOURCE_DIR}/3rd/NVCodecSDK/lib/${EXT_DIR}/nvencodeapi.a)
IF (WIN32)
    SET(nvcuvid_LIBRARY ${CMAKE_SOURCE_DIR}/3rd/NVCodecSDK/lib/${EXT_DIR}/nvcuvid.lib)
    SET(nvencodeapi_LIBRARY ${CMAKE_SOURCE_DIR}/3rd/NVCodecSDK/lib/${EXT_DIR}/nvencodeapi.lib)
ENDIF ()
SET(NVCodecSDK_LIBRARIES "${nvcuvid_LIBRARY}" "${nvencodeapi_LIBRARY}")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(NVCodecSDK DEFAULT_MSG
        NVCodecSDK_INCLUDE_DIRS NVCodecSDK_LIBRARIES)
MARK_AS_ADVANCED(NVCodecSDK_INCLUDE_DIRS NVCodecSDK_LIBRARIES)
