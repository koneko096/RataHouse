@echo off
echo Running RataHouse Tests...
echo.

echo [1/3] Running Interval Tests...
test_interval.exe
if %ERRORLEVEL% NEQ 0 (
    echo FAIL: Interval tests failed
    exit /b 1
)
echo.

echo [2/3] Running Device Tests...
test_device.exe
if %ERRORLEVEL% NEQ 0 (
    echo FAIL: Device tests failed
    exit /b 1
)
echo.

echo [3/3] Running Solver Integration Tests...
test_solver_integration.exe
if %ERRORLEVEL% NEQ 0 (
    echo FAIL: Solver tests failed
    exit /b 1
)
echo.

echo ========================================
echo ALL TESTS PASSED
echo ========================================
