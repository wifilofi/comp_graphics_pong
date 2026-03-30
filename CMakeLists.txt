cmake_minimum_required (VERSION 3.21)

if(POLICY CMP0162)
   cmake_policy(SET CMP0162 NEW)
endif()

project (pong
  DESCRIPTION "CMake example for Direct3D 11 Game (Win32) w/ DeviceResources using VCPKG"
  LANGUAGES CXX
  VERSION 1.0.0)

option(BUILD_TEST_TEMPLATE "Ignore warnings related to TODOs" OFF)

option(ENABLE_CODE_ANALYSIS "Use Static Code Analysis on build" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(CompilerAndLinker.cmake)

add_executable(${PROJECT_NAME} WIN32
        Engine/Basic/Components/Rendering.cpp
        Engine/Basic/Components/Rendering.h
        Engine/Basic/Shapes/Square.cpp
        Engine/Basic/Shapes/Square.h
        Engine/Input/Device.cpp
        Engine/Input/Device.h
        Engine/Input/Keyboard.h
        Engine/Input/Mouse.h
        Engine/Input/PC.cpp
        Engine/Input/PC.h
        Engine/Lib/Delegates.cpp
        Engine/Lib/Delegates.h
        Engine/Lib/TimeAccumulator.cpp
        Engine/Lib/TimeAccumulator.h
        Engine/Lib/Types.h
        Engine/Physics/Collide.cpp
        Engine/Physics/Collide.h
        Engine/Physics/CollideAble.h
        Engine/Physics/Move.cpp
        Engine/Physics/Move.h
        Engine/Physics/MoveAble.h
        Engine/Render/Able.h
        Engine/Render/Pipeline.cpp
        Engine/Render/Pipeline.h
        Engine/Render/Window.cpp
        Engine/Render/Window.h
        Engine/Update/Fixed.cpp
        Engine/Update/Fixed.h
        Engine/Update/FixedAble.h
        Engine/Game/Game.cpp
        Game/Pong/Ball.cpp
        Game/Pong/Wall.h
        Game/Pong/Wall.cpp
        Game/Pong/StickPlayer.h
        Game/Pong/StickPlayer.cpp
        Game/Pong/Stick.cpp
        Game/Pong/Stick.cpp
        Game/Pong/Counter.cpp
        Game/Pong/StickAI.cpp
        Game/Pong/BackgroundRenderer.h
        Game/Pong/BackgroundRenderer.cpp
        Game/Pong/TextRenderer.h
        Game/Pong/TextRenderer.cpp
        pch.h)

target_sources(${PROJECT_NAME}
    PRIVATE
        Main.cpp
)

if(NOT MINGW)
    target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d11.lib dxgi.lib dxguid.lib uuid.lib d3dcompiler.lib
    kernel32.lib user32.lib
    comdlg32.lib advapi32.lib shell32.lib
    ole32.lib oleaut32.lib
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Wall /EHsc /GR)
endif()

find_package(directxmath CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXMath)

# Uncomment if using the DirectX Tool Kit
find_package(directxtk CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXTK)

# Uncomment if using XAudio2Redist (required for MINGW)
# find_package(xaudio2redist CONFIG REQUIRED)
# target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::XAudio2Redist)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${COMPILER_DEFINES})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILER_SWITCHES})
target_link_options(${PROJECT_NAME} PRIVATE ${LINKER_SWITCHES})

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|IntelLLVM")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wpedantic -Wextra
        "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic"
        "-Wno-exit-time-destructors" "-Wno-global-constructors" "-Wno-language-extension-token"
        "-Wno-missing-prototypes" "-Wno-missing-variable-declarations" "-Wno-reserved-id-macro")

    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 16.0)
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-unsafe-buffer-usage")
    endif()

    if(BUILD_TEST_TEMPLATE)
        target_compile_options(${PROJECT_NAME} PRIVATE "-Wno-unused-value")
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wno-ignored-attributes)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
         "/wd4061" "/wd4365" "/wd4514" "/wd4571" "/wd4668" "/wd4710" "/wd4820" "/wd5039" "/wd5045")

    if(ENABLE_CODE_ANALYSIS)
        target_compile_options(${PROJECT_NAME} PRIVATE /analyze)
    endif()

    if(BUILD_TEST_TEMPLATE)
        target_compile_options(${PROJECT_NAME} PRIVATE "/wd4555")
    endif()
endif()

if(WIN32)
    if(${DIRECTX_ARCH} MATCHES "^arm")
        target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0A00)
    else()
        target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32_WINNT=0x0603)
    endif()
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>>,copy,true>
    $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND_EXPAND_LISTS
    )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Engine/Shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders
    )

add_custom_target(copy_fonts ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Game/Fonts $<TARGET_FILE_DIR:${PROJECT_NAME}>/Fonts
    DEPENDS ${PROJECT_NAME}
)
