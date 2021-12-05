cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(pml_log LANGUAGES CXX)

add_library(pml_log SHARED "src/log.cpp" )
set_target_properties(pml_log PROPERTIES DEBUG_POSTFIX "d")

target_include_directories(pml_log PUBLIC ${PROJECT_SOURCE_DIR}/include)

list(APPEND flags "-Wall" "-s" "-fexceptions" "-std=c++14")

if(CMAKE_BUILD_TYPE MATCHES Release)
   list(APPEND flags "-O3")
   target_compile_definitions(pml_log PUBLIC NDEBUG DLL_EXPORTS _MSL_STDINT_H)
else()
   list(APPEND flags "-g")
   target_compile_definitions(pml_log PUBLIC DEBUG DLL_EXPORTS _MSL_STDINT_H)
endif()


target_compile_options(pml_log PRIVATE ${flags})


#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_compile_definitions(pml_log PRIVATE __GNU__)
endif()

set_target_properties(pml_log PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/)

#linux specific
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	install(TARGETS pml_log LIBRARY DESTINATION /usr/local/lib)
endif()
