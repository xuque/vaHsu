cmake_minimum_required(VERSION 3.4.1)

project(vaHsu)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wall -v -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fno-exceptions -fPIC -DPIC -fpermissive -Wno-error=format-security -ffunction-sections -fdata-sections -fvisibility=hidden -fvisibility-inlines-hidden -Wdeprecated-declarations")

# third library
set(FFMPEG_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/ffmpeg/lib")
set(FFMPEG_LIB_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/libs/ffmpeg/include")
set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpegWrapper")

# base dir
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/common")
set(LOG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/log")
set(JNI_HELPER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/jni")
set(THREAD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/thread")

# include
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/")
include_directories(${INCLUDE_DIR})
include_directories(${COMMON_DIR})
include_directories(${LOG_DIR})
include_directories(${JNI_HELPER_DIR})
include_directories(${THREAD_DIR})
include_directories(${FFMPEG_DIR})
include_directories(${FFMPEG_LIB_INCLUDE})

# common
set(COMMON_SOURCES ${COMMON_DIR}/VaLock.cpp
                   ${COMMON_DIR}/VaUtils.cpp)
set(LOG_SOURCES ${LOG_DIR}/VaLogger.cpp)
set(THREAD_SOURCES ${THREAD_DIR}/VaXThread.cpp)
set(JNI_SOURCES ${JNI_HELPER_DIR}/JniHelper.cpp
                ${JNI_HELPER_DIR}/VaVideoEngine.cpp)

########
# test
set(TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/test/")
include_directories(${TEST_DIR})
set(TEST_SOURCES ${TEST_DIR}/testDecoder.cpp)
########

# wrapper
set(FFMPEG_WRAPPER_SOURCES ${FFMPEG_DIR}/ffmpegH264DecoderWrapper.cpp)

add_library(vaHsu SHARED
            ${COMMON_SOURCES}
            ${LOG_SOURCES}
            ${JNI_SOURCES}
            ${THREAD_SOURCES}
            ${FFMPEG_WRAPPER_SOURCES})

## 链接压缩库 libz
#find_library(ZIP_LIB z)
#target_link_libraries(vaHsu ${ZIP_LIB})
#link_libraries(z)
#

find_library(LOG_LIB log)
target_link_libraries(vaHsu ${LOG_LIB})

message("hsutest: ${FFMPEG_LIB_DIR}/${ANDROID_ABI}/libavcodec.so")

target_link_libraries(vaHsu "${FFMPEG_LIB_DIR}/${ANDROID_ABI}/libavcodec.so")
target_link_libraries(vaHsu "${FFMPEG_LIB_DIR}/${ANDROID_ABI}/libavfilter.so")
target_link_libraries(vaHsu "${FFMPEG_LIB_DIR}/${ANDROID_ABI}/libavformat.so")
target_link_libraries(vaHsu "${FFMPEG_LIB_DIR}/${ANDROID_ABI}/libavutil.so")
target_link_libraries(vaHsu "${FFMPEG_LIB_DIR}/${ANDROID_ABI}/libswresample.so")
target_link_libraries(vaHsu "${FFMPEG_LIB_DIR}/${ANDROID_ABI}/libswscale.so")
