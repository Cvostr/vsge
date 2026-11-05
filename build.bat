@echo off
setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if errorlevel 1 exit /b 1

cd /d "%~dp0"
if errorlevel 1 exit /b 1

set "PATH=C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja;%PATH%"

if /I "%~1"=="Release" goto build_release

if not exist "out\build\x64-Debug\build.ninja" (
    cmake -S . -B out\build\x64-Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
    if errorlevel 1 exit /b 1
)

if "%~1"=="" goto build_all
cmake --build out\build\x64-Debug --target "%~1" --parallel 4
exit /b %errorlevel%

:build_all
cmake --build out\build\x64-Debug --parallel 4
exit /b %errorlevel%

:build_release
set "BUILD_DIR=out\build\x64-Release"
cmake -S . -B "%BUILD_DIR%" -G Ninja -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 exit /b 1

cmake --build "%BUILD_DIR%" --target DemoGame --parallel 4
if errorlevel 1 exit /b 1

set "GAME_DIR=%CD%\%BUILD_DIR%\DemoGame"
set "ARCHIVE=%CD%\out\release\HouseOfShadows-win64.zip"
set "PACKAGE_DIR=%CD%\out\release\HouseOfShadows"
if not exist "%GAME_DIR%\DemoGame.exe" (
    echo ERROR: DemoGame.exe was not created.
    exit /b 1
)
if not exist "%GAME_DIR%\OpenAL32.dll" (
    echo ERROR: OpenAL32.dll is missing from the game output.
    exit /b 1
)
if not exist "%GAME_DIR%\Shaders" (
    echo ERROR: Shaders folder is missing from the game output.
    exit /b 1
)

cmake -E make_directory "out\release"
if errorlevel 1 exit /b 1

cmake -E remove_directory "%PACKAGE_DIR%"
if errorlevel 1 exit /b 1
cmake -E make_directory "%PACKAGE_DIR%"
if errorlevel 1 exit /b 1
cmake -E copy_if_different "%GAME_DIR%\DemoGame.exe" "%PACKAGE_DIR%"
if errorlevel 1 exit /b 1
cmake -E copy_if_different "%GAME_DIR%\OpenAL32.dll" "%PACKAGE_DIR%"
if errorlevel 1 exit /b 1
cmake -E copy_directory "%GAME_DIR%\Shaders" "%PACKAGE_DIR%\Shaders"
if errorlevel 1 exit /b 1
if not exist "%GAME_DIR%\MonoBinaries\lib\mono\4.5\mscorlib.dll" (
    echo ERROR: Mono managed runtime is missing from the game output.
    exit /b 1
)
cmake -E copy_directory "%GAME_DIR%\MonoBinaries" "%PACKAGE_DIR%\MonoBinaries"
if errorlevel 1 exit /b 1
if exist "%GAME_DIR%\sounds" (
    cmake -E copy_directory "%GAME_DIR%\sounds" "%PACKAGE_DIR%\sounds"
    if errorlevel 1 exit /b 1
)

powershell -NoProfile -ExecutionPolicy Bypass -Command "Compress-Archive -Path '%PACKAGE_DIR%\*' -DestinationPath '%ARCHIVE%' -Force"
if errorlevel 1 exit /b 1

echo Release package created: %ARCHIVE%
exit /b %errorlevel%
