cmake_minimum_required(VERSION 3.20)

project(PixelVault
    VERSION 1.0
    DESCRIPTION "DCT-based image steganography"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(OpenCV_DIR "C:/msys64/ucrt64/lib/cmake/opencv4")

find_package(OpenCV REQUIRED COMPONENTS core imgcodecs)

file(GLOB_RECURSE PIXELVAULT_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE PIXELVAULT_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)

add_executable(PixelVault
    ${PIXELVAULT_SOURCES}
    ${PIXELVAULT_HEADERS}
)

target_include_directories(PixelVault
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(PixelVault
    PRIVATE
        ${OpenCV_LIBS}
)

set_target_properties(PixelVault PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

foreach(CONFIG DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set_target_properties(PixelVault PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endforeach()

if(MSVC)
    target_compile_options(PixelVault PRIVATE
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Ob3>
        /W4
        /permissive-
    )
else()
    target_compile_options(PixelVault PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-DNDEBUG>
    )

    target_link_options(PixelVault PRIVATE
        $<$<CONFIG:Release>:-O3>
    )
endif()
