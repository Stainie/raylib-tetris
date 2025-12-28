@echo off
REM ============================================
REM Tetris Raylib - Web Build Script (Emscripten)
REM ============================================

set EMSDK_PATH=C:\Users\Stanko\Documents\Work\emsdk
set RAYLIB_PATH=C:\Users\Stanko\Documents\Work\raylib\src

REM Source files are in the same folder as this script
set PROJECT_PATH=C:\Users\Stanko\Documents\Work\tetris-raylib\tetris-raylib

REM Output to docs folder at solution level (for GitHub Pages)
set OUTPUT_DIR=C:\Users\Stanko\Documents\Work\tetris-raylib\docs

REM Activate Emscripten environment
echo Activating Emscripten...
call %EMSDK_PATH%\emsdk_env.bat

REM Create output directory if it doesn't exist
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

REM Navigate to project source folder
cd /d %PROJECT_PATH%

echo.
echo Current directory: %CD%
echo Looking for source files...
dir *.cpp
echo.
echo Building Tetris for Web...
echo.

REM Compile all source files
emcc -o %OUTPUT_DIR%\index.html ^
    main.cpp ^
    Game.cpp ^
    Board.cpp ^
    Tetromino.cpp ^
    GameUtils.cpp ^
    raylibCpp.cpp ^
    -Os ^
    -Wall ^
    -I. ^
    -I%RAYLIB_PATH% ^
    -L%RAYLIB_PATH% ^
    -lraylib ^
    -s USE_GLFW=3 ^
    -s ASYNCIFY ^
    -s TOTAL_MEMORY=67108864 ^
    -s ALLOW_MEMORY_GROWTH=1 ^
    -s FORCE_FILESYSTEM=1 ^
    -DPLATFORM_WEB ^
    --shell-file %PROJECT_PATH%\shell.html

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ============================================
    echo Build successful!
    echo Output files are in: %OUTPUT_DIR%
    echo.
    echo To test locally, run:
    echo   cd %OUTPUT_DIR%
    echo   python -m http.server 8000
    echo Then open http://localhost:8000 in your browser
    echo ============================================
) else (
    echo.
    echo Build failed with error code %ERRORLEVEL%
)

pause