cmake_minimum_required(VERSION 3.20)

project(Hello
    VERSION 1.0.0
    DESCRIPTION "Codename Hello — консольный собеседник на SDL3"
    LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

option(HELLO_BUILD_TESTS "Собирать юнит-тесты" ON)
option(HELLO_WERROR "Считать предупреждения ошибками" OFF)

# --- Поиск SDL3 -------------------------------------------------------
# На Windows легко оказаться с чужим тулчейном: например, scoop-сборкой
# MinGW, в которой SDL3 нет. Тогда find_package падает невнятным
# «Could not find a package configuration file». Поэтому сначала
# подсказываем CMake типичные места MSYS2, а если SDL3 так и не
# нашёлся — объясняем, что делать.
#
# Текст ошибки держим короткими строками: CMake переносит по байтам,
# а кириллица в UTF-8 занимает два байта на символ.

if(NOT SDL3_DIR)
    foreach(hello_prefix
            "$ENV{MSYSTEM_PREFIX}"
            "$ENV{SCOOP}/apps/msys2/current/ucrt64"
            "$ENV{SCOOP}/apps/msys2/current/mingw64"
            "C:/msys64/ucrt64"
            "C:/msys64/mingw64"
            "/ucrt64"
            "/mingw64")
        if(hello_prefix AND EXISTS "${hello_prefix}/lib/cmake/SDL3")
            list(APPEND CMAKE_PREFIX_PATH "${hello_prefix}")
        endif()
    endforeach()
    unset(hello_prefix)
endif()

find_package(SDL3 CONFIG QUIET)
if(NOT SDL3_FOUND)
    message(FATAL_ERROR
        "\n"
        "SDL3 не найден.\n"
        "\n"
        "Компилятор:\n"
        "  ${CMAKE_C_COMPILER}\n"
        "CMake:\n"
        "  ${CMAKE_COMMAND}\n"
        "\n"
        "Похоже, это не MSYS2.\n"
        "SDL3 стоит в MSYS2 (ucrt64),\n"
        "а тулчейн взят другой —\n"
        "например, scoop MinGW,\n"
        "где SDL3 нет.\n"
        "\n"
        "Способ 1: собрать в MSYS2.\n"
        "  pacman -S --needed\n"
        "      mingw-w64-ucrt-x86_64-gcc\n"
        "      mingw-w64-ucrt-x86_64-cmake\n"
        "      mingw-w64-ucrt-x86_64-ninja\n"
        "      mingw-w64-ucrt-x86_64-sdl3\n"
        "      mingw-w64-ucrt-x86_64-sdl3-ttf\n"
        "  cmake -S . -B build -G Ninja\n"
        "  cmake --build build\n"
        "\n"
        "Способ 2: указать префикс.\n"
        "  cmake -S . -B build -G Ninja\n"
        "    -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64\n"
        "\n"
        "Способ 3: build.cmd в корне.\n"
        "  Он сам найдёт MSYS2.\n")
endif()

find_package(SDL3_ttf CONFIG QUIET)
if(NOT SDL3_ttf_FOUND)
    message(FATAL_ERROR
        "\n"
        "SDL3_ttf не найден\n"
        "(SDL3 при этом найден).\n"
        "\n"
        "  MSYS2:  pacman -S mingw-w64-ucrt-x86_64-sdl3-ttf\n"
        "  Ubuntu: apt install libsdl3-ttf-dev\n"
        "  macOS:  brew install sdl3_ttf\n")
endif()

# --- Общая библиотека логики (без SDL) --------------------------------
add_library(hello_core STATIC
    src/chat.c
    src/config.c
    src/export.c
    src/input.c
    src/markov.c
    src/pet.c
    src/text_metrics.c
    src/utf8.c
    src/viewkind.c)

target_include_directories(hello_core PUBLIC src)
target_compile_features(hello_core PUBLIC c_std_11)

# libm: на glibc (Linux/macOS) математика вынесена в отдельную библиотеку.
# В MSYS2/MinGW она входит в CRT, но явная линковка там безвредна.
if(NOT MSVC)
    find_library(HELLO_LIBM m)
    if(HELLO_LIBM)
        target_link_libraries(hello_core PUBLIC ${HELLO_LIBM})
    endif()
endif()

if(MSVC)
    target_compile_options(hello_core PRIVATE /W4)
else()
    target_compile_options(hello_core PRIVATE
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-sign-conversion)
    if(HELLO_WERROR)
        target_compile_options(hello_core PRIVATE -Werror)
    endif()
endif()

# --- Приложение -------------------------------------------------------
add_executable(hello
    src/main.c
    src/app.c
    src/platform.c
    src/render.c
    src/theme.c
    src/view.c)

configure_file(src/version.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h @ONLY)

target_include_directories(hello PRIVATE
    src
    ${CMAKE_CURRENT_BINARY_DIR}/generated)

if(TARGET SDL3::SDL3main)
    target_link_libraries(hello PRIVATE SDL3::SDL3main)
endif()

target_link_libraries(hello PRIVATE
    hello_core
    SDL3::SDL3
    SDL3_ttf::SDL3_ttf)

target_compile_features(hello PRIVATE c_std_11)

if(MSVC)
    target_compile_options(hello PRIVATE /W4)
    target_link_options(hello PRIVATE /SUBSYSTEM:WINDOWS)
else()
    target_compile_options(hello PRIVATE
        -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-sign-conversion)
    if(HELLO_WERROR)
        target_compile_options(hello PRIVATE -Werror)
    endif()
endif()

# --- Иконка приложения (Windows) --------------------------------------
if(WIN32)
    enable_language(RC)
    set(HELLO_ICON_RC ${CMAKE_CURRENT_BINARY_DIR}/generated/hello.rc)
    configure_file(assets/hello.rc ${HELLO_ICON_RC} COPYONLY)
    target_sources(hello PRIVATE ${HELLO_ICON_RC})
    set_source_files_properties(${HELLO_ICON_RC} PROPERTIES
        COMPILE_FLAGS "-I${CMAKE_CURRENT_SOURCE_DIR}/assets")
endif()

# --- Установка --------------------------------------------------------
include(GNUInstallDirs)

install(TARGETS hello RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES
    assets/fonts/DejaVuSansMono-Bold.ttf
    assets/fonts/LICENSE
    assets/fonts/AUTHORS
    DESTINATION ${CMAKE_INSTALL_DATADIR}/hello/fonts)
install(FILES README.md LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})

# --- Тесты ------------------------------------------------------------
if(HELLO_BUILD_TESTS)
    enable_testing()

    add_executable(hello_tests
        tests/test_main.c
        tests/test_utf8.c
        tests/test_chat.c
        tests/test_input.c
        tests/test_config.c
        tests/test_metrics.c
        tests/test_markov.c
        tests/test_export.c
        tests/test_pet.c
        tests/test_viewkind.c
        tests/test_theme.c
        src/theme.c)

    target_link_libraries(hello_tests PRIVATE hello_core SDL3::SDL3)
    target_include_directories(hello_tests PRIVATE src tests)
    target_compile_features(hello_tests PRIVATE c_std_11)

    add_test(NAME hello_tests COMMAND hello_tests)
endif()
