@echo off
REM ============================================
REM Build Raylib for Web (Emscripten)
REM Run this ONCE before building your game
REM ============================================

set EMSDK_PATH=C:\Users\Stanko\Documents\Work\emsdk
set RAYLIB_PATH=C:\Users\Stanko\Documents\Work\raylib\src

REM Activate Emscripten environment
echo Activating Emscripten...
call %EMSDK_PATH%\emsdk_env.bat

REM Navigate to raylib source
cd /d %RAYLIB_PATH%

echo.
echo Building Raylib for Web...
echo.

REM Compile raylib modules for web
emcc -c rcore.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
if %ERRORLEVEL% NEQ 0 goto :error

emcc -c rshapes.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
if %ERRORLEVEL% NEQ 0 goto :error

emcc -c rtextures.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
if %ERRORLEVEL% NEQ 0 goto :error

emcc -c rtext.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
if %ERRORLEVEL% NEQ 0 goto :error

emcc -c rmodels.c -Os -Wall -DPLATFORM_WEB -DGRAPHICS_API_OPENGL_ES2
if %ERRORLEVEL% NEQ 0 goto :error

emcc -c utils.c -Os -Wall -DPLATFORM_WEB
if %ERRORLEVEL% NEQ 0 goto :error

emcc -c raudio.c -Os -Wall -DPLATFORM_WEB
if %ERRORLEVEL% NEQ 0 goto :error

REM Create static library
echo.
echo Creating libraylib.a...
emar rcs libraylib.a rcore.o rshapes.o rtextures.o rtext.o rmodels.o utils.o raudio.o

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ============================================
    echo Raylib web build successful!
    echo Library created: %RAYLIB_PATH%\libraylib.a
    echo.
    echo You can now run build_web.bat to build your game.
    echo ============================================
) else (
    goto :error
)

goto :end

:error
echo.
echo Build failed!

:end
pause