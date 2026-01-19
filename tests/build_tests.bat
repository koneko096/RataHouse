@echo off
REM Build script for RataHouse unit tests

echo ========================================
echo Building RataHouse Unit Tests
echo ========================================
echo.

REM Setup VS Environment
call "C:\Program Files\Microsoft Visual Studio\18\Community\Common7\Tools\VsDevCmd.bat"
if %errorlevel% neq 0 (
    echo "Failed to load VsDevCmd.bat"
    exit /b %errorlevel%
)

echo Building Interval Tests...
cl /EHsc /I"../include" test_interval.cpp ../src/Interval.cpp /Fe:test_interval.exe
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to build Interval tests
    exit /b 1
)
echo Interval tests built successfully
echo.

echo Building Device Tests...
cl /EHsc /I"../include" test_device.cpp ../src/Device.cpp ../src/Interval.cpp /Fe:test_device.exe
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to build Device tests
    exit /b 1
)
echo Device tests built successfully
echo.

echo Building Solver Integration Tests...
cl /EHsc /I"../include" test_solver_integration.cpp ../src/Solver.cpp ../src/Device.cpp ../src/Interval.cpp /Fe:test_solver_integration.exe
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Failed to build Solver tests
    exit /b 1
)
echo Solver tests built successfully
echo.

echo ========================================
echo All tests built successfully!
echo ========================================
