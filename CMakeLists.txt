cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(pml_log LANGUAGES CXX VERSION 1.3.0)
set(CMAKE_INSTALL_PREFIX "/usr/local")

execute_process(COMMAND ${CMAKE_COMMAND} -DNAMESPACE=log -DMAJOR=${PROJECT_VERSION_MAJOR} -DMINOR=${PROJECT_VERSION_MINOR} -DPATCH=${PROJECT_VERSION_PATCH} -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)



add_library(pml_log SHARED "src/log.cpp" "src/logtofile.cpp" ${CMAKE_BINARY_DIR}/src/log_version.cpp)
set_target_properties(pml_log PROPERTIES DEBUG_POSTFIX "d")


target_include_directories(pml_log PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(pml_log PUBLIC ${CMAKE_BINARY_DIR}/include)

list(APPEND flags "-Wall" "-s" "-fexceptions" "-std=c++17")

if(CMAKE_BUILD_TYPE MATCHES Debug)
   list(APPEND flags "-g")
   target_compile_definitions(pml_log PUBLIC DEBUG DLL_EXPORTS _MSL_STDINT_H)
else()
   list(APPEND flags "-O3" "-s")
   target_compile_definitions(pml_log PUBLIC NDEBUG DLL_EXPORTS _MSL_STDINT_H)
endif()


target_compile_options(pml_log PRIVATE ${flags})


#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_compile_definitions(pml_log PRIVATE __GNU__)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(pml_log PRIVATE _WIN32)
endif()

set_target_properties(pml_log PROPERTIES DEBUG_POSTFIX "d")
set_target_properties(pml_log PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)
set_target_properties(pml_log PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})

#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(TARGETS pml_log LIBRARY DESTINATION lib)
endif()
