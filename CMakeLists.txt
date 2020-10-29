cmake_minimum_required(VERSION 3.12)
project(myintegration)

find_package(slsDetectorPackage 5.0 REQUIRED)

add_executable(e1-config e1-config.cpp)
target_link_libraries(e1-config slsDetectorShared)

add_executable(e2-result e2-result.cpp)
target_link_libraries(e2-result slsDetectorShared)

add_executable(e3-time e3-time.cpp)
target_link_libraries(e3-time slsDetectorShared)

add_executable(e4-rxr e4-receiver_callbacks.cpp)
target_link_libraries(e4-rxr slsReceiverShared slsSupportStatic pthread)


# Copy config files to bin dir. 
set( CONFIG_FILES
    one_det_no_receiver.config  
    one_det_minimal.config 
)

foreach(FILE ${CONFIG_FILES})
    configure_file( config/${FILE}  
    ${CMAKE_BINARY_DIR}/${FILE}  )
endforeach(FILE ${PYTHON_FILES})