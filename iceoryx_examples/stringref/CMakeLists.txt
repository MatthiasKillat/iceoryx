cmake_minimum_required(VERSION 3.5)
project(stringref)

find_package(iceoryx_utils CONFIG REQUIRED)

add_executable(stringref ./stringref_main.cpp)
target_link_libraries(stringref
    iceoryx_utils::iceoryx_utils
)

set_target_properties(stringref PROPERTIES 
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 11
    POSITION_INDEPENDENT_CODE ON
)

