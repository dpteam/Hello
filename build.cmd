@echo off
rem ---------------------------------------------------------------------------
rem Codename Hello - build helper for Windows.
rem
rem Sets up the MSYS2 UCRT64 environment and builds. Use this when you are in
rem plain PowerShell/CMD: the standalone CMake and scoop MinGW there have no
rem SDL3, which is only installed inside MSYS2.
rem
rem   build.cmd            configure + build (Release)
rem   build.cmd debug      configure + build (Debug)
rem   build.cmd test       build, then run unit tests and self-check
rem   build.cmd clean      remove the build directory
rem
rem Keep this file ASCII-only: cmd.exe reads it in the OEM code page.
rem ---------------------------------------------------------------------------
setlocal EnableDelayedExpansion

set "ROOT=%~dp0"
set "ROOT=%ROOT:~0,-1%"
set "BUILD=%ROOT%\build"
set "MODE=%~1"
if "%MODE%"=="" set "MODE=release"

if /i "%MODE%"=="clean" (
    if exist "%BUILD%" rmdir /s /q "%BUILD%"
    echo Removed %BUILD%
    exit /b 0
)

rem --- locate MSYS2 ----------------------------------------------------------
set "MSYS="
if defined MSYSTEM_PREFIX if exist "%MSYSTEM_PREFIX%\bin\cmake.exe" set "MSYS=%MSYSTEM_PREFIX%"
if not defined MSYS if exist "C:\msys64\ucrt64\bin\cmake.exe" set "MSYS=C:\msys64\ucrt64"
if not defined MSYS if defined SCOOP if exist "%SCOOP%\apps\msys2\current\ucrt64\bin\cmake.exe" set "MSYS=%SCOOP%\apps\msys2\current\ucrt64"
if not defined MSYS if exist "%USERPROFILE%\scoop\apps\msys2\current\ucrt64\bin\cmake.exe" set "MSYS=%USERPROFILE%\scoop\apps\msys2\current\ucrt64"

if not defined MSYS (
    echo ERROR: MSYS2 UCRT64 not found.
    echo.
    echo Install it, then run:
    echo   pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake ^
    echo       mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-sdl3 ^
    echo       mingw-w64-ucrt-x86_64-sdl3-ttf
    echo.
    echo Or pass the prefix explicitly:
    echo   set MSYSTEM_PREFIX=C:\path\to\msys64\ucrt64
    exit /b 1
)

if not exist "%MSYS%\bin\SDL3.dll" (
    echo WARNING: SDL3.dll not found in %MSYS%\bin
    echo   pacman -S mingw-w64-ucrt-x86_64-sdl3 mingw-w64-ucrt-x86_64-sdl3-ttf
)

echo Using toolchain: %MSYS%
set "PATH=%MSYS%\bin;%PATH%"

rem --- configure -------------------------------------------------------------
if /i "%MODE%"=="debug" (
    set "CFG=Debug"
) else (
    set "CFG=Release"
)

cmake -S "%ROOT%" -B "%BUILD%" -G Ninja -DCMAKE_BUILD_TYPE=!CFG! -DCMAKE_PREFIX_PATH=%MSYS%
if errorlevel 1 exit /b 1

rem --- build -----------------------------------------------------------------
cmake --build "%BUILD%"
if errorlevel 1 exit /b 1

if /i not "%MODE%"=="test" (
    echo.
    echo Built: %BUILD%\hello.exe
    exit /b 0
)

rem --- test ------------------------------------------------------------------
echo.
echo === unit tests ===
"%BUILD%\hello_tests.exe"
if errorlevel 1 exit /b 1

echo.
echo === input self-check ===
set "SDL_VIDEODRIVER=dummy"
"%BUILD%\hello.exe" --selftest
set "SDL_VIDEODRIVER="
if errorlevel 1 exit /b 1

echo.
echo All checks passed.
exit /b 0
