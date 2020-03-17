cmake_minimum_required(VERSION 3.12)
project(myintegration)

find_package(slsDetectorPackage 5.0 REQUIRED)
add_executable(e1-config e1-config.cpp)
target_link_libraries(e1-config slsDetectorShared)


configure_file( one_det_no_receiver.config  
    ${CMAKE_BINARY_DIR}/one_det_no_receiver.config

)