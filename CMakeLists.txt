cmake_minimum_required(VERSION 3.0.0)
project(RTSP_Client VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_BUILD_TYPE Debug)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
SET(CMAKE_CXX_STANDARD_REQUIRED 17)

# 寻找子串，不存在则返回-1
string(FIND ${CMAKE_CXX_COMPILER} "aarch64-linux-gnu-g++" S_index)

if("${S_index}" STREQUAL "-1")
    message("Compile on PC platform")
    set(COMPILE_TYPR x86_64bit)

    include_directories(./include)

    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})

        # FFMPEG
    include_directories(/usr/include/x86_64-linux-gnu/libavcodec)
    include_directories(/usr/include/x86_64-linux-gnu/libavutil)
    include_directories(/usr/include/x86_64-linux-gnu/libavformat)
    include_directories(/usr/include/x86_64-linux-gnu/libswscale)
    link_directories(/usr/lib/x86_64-linux-gnu)

    add_executable(h264_decoder src/h264_decoder.cc include/H264Decoder.h)

    target_link_libraries(h264_decoder
        avcodec
        avutil
        avformat
        swscale
        avdevice
        avfilter
        postproc
        swresample
    )

    add_executable(hw_h264_decoder src/H264_Decoder_HW.cc include/H264Decoder_HW.h)
    target_link_libraries(hw_h264_decoder
        avcodec 
        avdevice 
        avfilter 
        avformat 
        avutil 
        postproc 
        swresample 
        swscale
    )

else()
    message("Cross compiling!!!")
    set(COMPILE_TYPR arm_64bit)


    
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
