cmake_minimum_required(VERSION 3.4.1)

project(vaHsu)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wall -v -Wl,--gc-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frtti -fno-exceptions -fPIC -DPIC -fpermissive -Wno-error=format-security -ffunction-sections -fdata-sections -fvisibility=hidden -fvisibility-inlines-hidden")

# third library
set(FFMPEG_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/ffmpeg/lib")

set(JNI_HELPER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/jni")
set(FFMPEG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/ffmpegWrapper")

# 头文件目录
include_directories("${JNI_HELPER_DIR}")
include_directories("${FFMPEG_DIR}")

set(FFMPEG_WRAPPER_SOURCES ${FFMPEG_DIR}/ffmpegWrapper.cpp)
set(JNI_SOURCES ${JNI_HELPER_DIR}/JniHelper.cpp)

add_library(vaHsu SHARED
            ${FFMPEG_WRAPPER_SOURCES}
            ${JNI_SOURCES})

## 链接压缩库 libz
#find_library(ZIP_LIB z)
#target_link_libraries(vaHsu ${ZIP_LIB})
#link_libraries(z)
#

find_library(LOG_LIB log)
target_link_libraries(vaHsu ${LOG_LIB})

target_link_libraries(vaHsu "${FFMPEG_LIB_DIR}/${ANDROID_ABI}/libavcodec.so")
