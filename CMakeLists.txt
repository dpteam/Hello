cmake_minimum_required(VERSION 3.20)

project(Hello
    VERSION 1.0.0
    DESCRIPTION "Codename Hello — console собеседник на SDL2"
    LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

option(HELLO_BUILD_TESTS "Собирать юнит-тесты" ON)
option(HELLO_WERROR "Считать предупреждения ошибками" OFF)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

# --- Общая библиотека логики (без SDL) --------------------------------
add_library(hello_core STATIC
    src/chat.c
    src/config.c
    src/input.c
    src/markov.c
    src/text_metrics.c
    src/utf8.c)

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

if(TARGET SDL2::SDL2main)
    target_link_libraries(hello PRIVATE SDL2::SDL2main)
endif()

target_link_libraries(hello PRIVATE
    hello_core
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf)

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
        tests/test_markov.c)

    target_link_libraries(hello_tests PRIVATE hello_core)
    target_include_directories(hello_tests PRIVATE src tests)
    target_compile_features(hello_tests PRIVATE c_std_11)

    add_test(NAME hello_tests COMMAND hello_tests)
endif()
