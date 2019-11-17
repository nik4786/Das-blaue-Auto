project(object_detection_yolo)    

SET(DARKNET_ROOT "/home/aadc/darknet-cpp-master/")
SET(darknet_cpp_SOURCE_DIR "/home/aadc/darknet-cpp-master/darknet_cpp/" )

include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories ("${DARKNET_ROOT}/include")
include_directories (${CUDA_INCLUDE_DIRS})
include_directories (${darknet_cpp_SOURCE_DIR}/src)
link_directories (${darknet_cpp_SOURCE_DIR}/src)

adtf_add_filter(${PROJECT_NAME}
    stdafx.h
../owndesc.description
    ObjectDetect.cpp
    ObjectDetect.h
)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS}
/home/aadc/darknet-cpp-master/libdarknet.a
/usr/local/lib/libdarknet_cpp.so )

adtf_install_plugin(${PROJECT_NAME} bin)

#install dlls for opencv to binary folder 
if(WIN32)
    install(FILES "${OpenCV_DIR}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/bin/opencv_world${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}d.dll" 
    DESTINATION ${CMAKE_INSTALL_BINARY}/debug CONFIGURATIONS Debug)
    install(FILES "${OpenCV_DIR}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/bin/opencv_world${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll"
    DESTINATION ${CMAKE_INSTALL_BINARY} CONFIGURATIONS RelWithDebInfo Release)
endif(WIN32)

adtf_create_plugindescription(
    TARGET
        ${PROJECT_NAME}
    PLUGIN_SUBDIR
        "bin")        

set_property(TARGET ${PROJECT_NAME} PROPERTY FOLDER algorithms)
set_property(TARGET ${PROJECT_NAME}_pdgen PROPERTY FOLDER algorithms)
