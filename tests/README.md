# RataHouse Unit Tests

## Overview

This directory contains unit tests for the RataHouse device scheduling application.

## Test Files

- **test_interval.cpp** - Tests for the Interval class
  - Constructor initialization
  - IsInside() boundary checking
  - IsCollide() collision detection
  - Increment/decrement operators
  - Comparison operators
  - Edge cases

- **test_device.cpp** - Tests for the Device class
  - Constructor initialization
  - IsWajib() mandatory checking
  - IsPermitted() validation logic
  - Comparison operator for sorting
  - Property assignments

## Running Tests

### Method 1: Using Build Scripts (Recommended)

#### Step-by-Step Instructions:

1. **Open Visual Studio Developer Command Prompt**
   - Press `Windows Key`
   - Type "Developer Command Prompt"
   - Select "Developer Command Prompt for VS 2022" (or your VS version)
   - This opens a command prompt with the C++ compiler (`cl.exe`) in the PATH

2. **Navigate to the tests directory**
   ```cmd
   cd C:\Users\laser\OneDrive\Documents\code\RataHouse\tests
   ```

3. **Build all tests**
   ```cmd
   build_tests.bat
   ```
   This compiles both test executables.

4. **Run all tests**
   ```cmd
   run_tests.bat
   ```
   This runs both test suites and shows a summary.

#### Expected Output:
```
========================================
Running RataHouse Unit Tests
========================================

Running Interval Tests...
----------------------------------------
...
✓ ALL TESTS PASSED!

Running Device Tests...
----------------------------------------
...
✓ ALL TESTS PASSED!

========================================
Test Summary
========================================
Interval Tests: PASSED
Device Tests: PASSED
========================================

✓ ALL TESTS PASSED!
```

### Method 2: Manual Compilation

If you prefer to compile and run tests individually:

#### Compile and Run Interval Tests

```cmd
# Compile
cl /EHsc /I.. test_interval.cpp ..\src\Interval.cpp /Fe:test_interval.exe

# Run
test_interval.exe
```

#### Compile and Run Device Tests

```cmd
# Compile
cl /EHsc /I.. test_device.cpp ..\src\Device.cpp ..\src\Interval.cpp /Fe:test_device.exe

# Run
test_device.exe
```

### Method 3: Using Regular PowerShell/CMD

If you don't want to use Developer Command Prompt, you can use the full path to the compiler:

```cmd
"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\<version>\bin\Hostx64\x64\cl.exe" /EHsc /I.. test_interval.cpp ..\src\Interval.cpp /Fe:test_interval.exe
```

**Note:** Replace `<version>` with your actual MSVC version number.

### Alternative: Using g++ (if MinGW installed)

```bash
# Interval tests
g++ -std=c++11 -I.. test_interval.cpp ../src/Interval.cpp -o test_interval.exe
./test_interval.exe

# Device tests
g++ -std=c++11 -I.. test_device.cpp ../src/Device.cpp ../src/Interval.cpp -o test_device.exe
./test_device.exe
```

## Test Data

The `test_data/` directory contains sample input files for testing:
- **test_input.txt** - Valid input file with known configuration

## Expected Output

When all tests pass, you should see:

```
========================================
  INTERVAL CLASS UNIT TESTS
========================================

Testing Interval Constructor...
Testing Interval::IsInside()...
Testing Interval::IsCollide()...
...

========================================
  TEST RESULTS
========================================
Tests Passed: XX
Tests Failed: 0
Total Tests:  XX

✓ ALL TESTS PASSED!
```

## Adding New Tests

To add new tests:

1. Create a new test function using the `TEST(name)` macro
2. Use assertion macros:
   - `ASSERT_TRUE(condition)` - Assert condition is true
   - `ASSERT_FALSE(condition)` - Assert condition is false
   - `ASSERT_EQ(a, b)` - Assert a equals b
3. Call your test function in `main()`

Example:
```cpp
TEST(test_my_feature) {
    cout << "Testing my feature..." << endl;
    ASSERT_TRUE(my_function() == expected_value);
}
```
