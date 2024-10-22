cmake_minimum_required(VERSION 3.14)
project(GraphicsConverter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent)

function(fetch_content_if_not_exists name git_repo git_tag)
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/vendor/${name})
        FetchContent_Declare(
            ${name}
            GIT_REPOSITORY ${git_repo}
            GIT_TAG ${git_tag}
            SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/${name}
        )
        FetchContent_MakeAvailable(${name})
    endif()
endfunction()

fetch_content_if_not_exists(spdlog https://github.com/gabime/spdlog.git v1.14.1)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog)
fetch_content_if_not_exists(fmt https://github.com/fmtlib/fmt.git 11.0.1)
fetch_content_if_not_exists(imgui https://github.com/ocornut/imgui.git v1.90.9)
fetch_content_if_not_exists(glfw https://github.com/glfw/glfw.git 3.4)
fetch_content_if_not_exists(stb https://github.com/nothings/stb.git master)
fetch_content_if_not_exists(nativefiledialog https://github.com/mlabbe/nativefiledialog.git master)
fetch_content_if_not_exists(googletest https://github.com/google/googletest.git v1.14.0)

set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "Build shared library" FORCE)
set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "Use external fmt library instead of bundled" FORCE)

set(IMGUI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_demo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_draw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_tables.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/imgui_widgets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends/imgui_impl_opengl3.cpp
)

set(NFD_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nativefiledialog/src/nfd_common.c
)

if(WIN32)
    list(APPEND NFD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nativefiledialog/src/nfd_win.cpp)
elseif(APPLE)
    list(APPEND NFD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nativefiledialog/src/nfd_cocoa.m)
else()
    list(APPEND NFD_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nativefiledialog/src/nfd_gtk.c)
endif()

find_package(OpenGL REQUIRED)
find_package(fmt REQUIRED)
find_package(glfw3 REQUIRED)

include(GoogleTest)

add_executable(GraphicsConverter
    src/Main.cpp
    src/Converter.cpp
    src/ColorReducer.cpp
    src/KoalaConverter.cpp
    src/STDImage.cpp
    src/Dithering.cpp
    src/GuiLogSink.cpp
    src/Logger.cpp
    ${IMGUI_SOURCES}
    ${NFD_SOURCES}
)

add_executable(GraphicsConverterTests
    tests/ColorReducerTests.cpp
    tests/ConverterTests.cpp
)

add_library(GraphicsConverterLib STATIC
    src/Converter.cpp
    src/ColorReducer.cpp
    src/KoalaConverter.cpp
    src/STDImage.cpp
    src/Dithering.cpp
    src/GuiLogSink.cpp
    src/Logger.cpp
)

target_include_directories(GraphicsConverterLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nativefiledialog/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include
)

target_include_directories(GraphicsConverter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/nativefiledialog/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

target_link_libraries(GraphicsConverter PRIVATE
    glfw
    OpenGL::GL
    spdlog
    fmt::fmt
)

target_link_libraries(GraphicsConverterTests PRIVATE
    gtest
    gtest_main
)

target_link_libraries(GraphicsConverterLib PUBLIC
    spdlog
    fmt::fmt
)

gtest_discover_tests(GraphicsConverterTests)

if(WIN32)
    target_compile_definitions(GraphicsConverter PRIVATE -DWIN32)
elseif(APPLE)
    target_link_libraries(GraphicsConverter PRIVATE "-framework Cocoa")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    target_link_libraries(GraphicsConverter PRIVATE ${GTK3_LIBRARIES})
    target_include_directories(GraphicsConverter PRIVATE ${GTK3_INCLUDE_DIRS})
    target_compile_options(GraphicsConverter PRIVATE ${GTK3_CFLAGS_OTHER})
endif()

# Link GraphicsConverterLib to your main executable
target_link_libraries(GraphicsConverter PRIVATE GraphicsConverterLib)

# Link GraphicsConverterLib to your test executable
target_link_libraries(GraphicsConverterTests PRIVATE GraphicsConverterLib)

target_compile_definitions(GraphicsConverter PRIVATE NFD_STATIC)

target_sources(GraphicsConverter PRIVATE ${NFD_SOURCES})