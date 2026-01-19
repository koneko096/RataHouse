@echo off
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
if %errorlevel% neq 0 (
    echo "Failed to load VsDevCmd.bat."
    exit /b %errorlevel%
)
cl.exe /EHsc /Fe:debug_solver.exe debug_solver.cpp ../src/Device.cpp ../src/Interval.cpp /I"../include"
if %errorlevel% neq 0 exit /b %errorlevel%
echo Build successful!
debug_solver.exe
