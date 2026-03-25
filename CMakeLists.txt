cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
include(addexternal.cmake)

project(pml_log LANGUAGES CXX VERSION 1.4.2)
set(CMAKE_INSTALL_PREFIX "/usr/local")

SET(DIR_BASE ${CMAKE_SOURCE_DIR}/external CACHE STRING "base location for libraries")
SET(DIR_QUEUE ${DIR_BASE}/concurrentqueue CACHE STRING "location of concurrentqueue")

execute_process(COMMAND ${CMAKE_COMMAND} -DNAMESPACE=log -DMAJOR=${PROJECT_VERSION_MAJOR} -DMINOR=${PROJECT_VERSION_MINOR} -DPATCH=${PROJECT_VERSION_PATCH} -DOUTPUT=${CMAKE_BINARY_DIR} -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)



add_external_library(concurrentqueue ${DIR_QUEUE} "cameron314/concurrentqueue.git" "master" FALSE "CMakeLists.txt")

if(NOT TARGET pml_log)
add_library(pml_log SHARED "src/log.cpp" "src/logtofile.cpp" ${CMAKE_BINARY_DIR}/src/log_version.cpp)
set_target_properties(pml_log PROPERTIES DEBUG_POSTFIX "d")

target_include_directories(pml_log PUBLIC ${PROJECT_SOURCE_DIR}/include
                                          ${CMAKE_BINARY_DIR}/include
                                          ${DIR_QUEUE})

list(APPEND flags "-Wall" "-s" "-fexceptions" "-std=c++17")

if(CMAKE_BUILD_TYPE MATCHES Debug)
   list(APPEND flags "-g")
   target_compile_definitions(pml_log PUBLIC DEBUG DLL_EXPORTS _MSL_STDINT_H LOG_DLL)
else()
   list(APPEND flags "-O3" "-s")
   target_compile_definitions(pml_log PUBLIC NDEBUG DLL_EXPORTS _MSL_STDINT_H LOG_DLL)
endif()

target_compile_options(pml_log PRIVATE ${flags})

#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_compile_definitions(pml_log PRIVATE __GNU__)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(pml_log PRIVATE _WIN32)
endif()

set_target_properties(pml_log PROPERTIES DEBUG_POSTFIX "d")
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set_target_properties(pml_log PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/
            ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/
        )
    else()
set_target_properties(pml_log PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)
    endif()
set_target_properties(pml_log PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(TARGETS pml_log LIBRARY DESTINATION lib)
endif()


# demo executable to quickly verify logging
add_executable(pml_log_demo demo/main.cpp)
target_include_directories(pml_log_demo PRIVATE ${PROJECT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/include)
target_link_libraries(pml_log_demo PRIVATE pml_log)
set_target_properties(pml_log_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

endif()