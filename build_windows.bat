@echo off
setlocal
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp TaskManager.cpp UI.cpp -o StudentTaskScheduler.exe
if errorlevel 1 (
    echo.
    echo Build failed. Make sure g++ is installed and available in PATH.
    pause
    exit /b 1
)
echo.
echo Build successful. Starting Student Task Scheduler...
StudentTaskScheduler.exe
pause
