# Quick Start Guide: Running RataHouse Unit Tests

## 🚀 Quick Start (3 Steps)

### Step 1: Open Developer Command Prompt
![Windows Key → Type "Developer Command Prompt" → Select your VS version]

**Where to find it:**
- Press `Windows Key`
- Type: `Developer Command Prompt`
- Click: `Developer Command Prompt for VS 2022` (or your version)

### Step 2: Navigate to Tests Directory
```cmd
cd C:\Users\laser\OneDrive\Documents\code\RataHouse\tests
```

### Step 3: Build and Run
```cmd
build_tests.bat
run_tests.bat
```

## ✅ Success!

You should see:
```
✓ ALL TESTS PASSED!
```

---

## 📋 What Gets Tested?

### Interval Class (27 tests)
- ✓ Constructor
- ✓ Boundary checking
- ✓ Collision detection
- ✓ Operators

### Device Class (17 tests)
- ✓ Validation logic
- ✓ Mandatory checking
- ✓ Sorting
- ✓ Properties

---

## ❓ Troubleshooting

### "cl is not recognized"
**Problem:** Not using Developer Command Prompt  
**Solution:** Use Developer Command Prompt, not regular CMD/PowerShell

### "Cannot find test files"
**Problem:** Wrong directory  
**Solution:** Make sure you're in the `tests/` directory

### Build errors
**Problem:** Missing source files  
**Solution:** Ensure all files are in the correct locations:
- `tests/test_interval.cpp`
- `tests/test_device.cpp`
- `src/Interval.cpp`
- `src/Device.cpp`

---

## 📚 More Information

See `README.md` for:
- Detailed compilation options
- Manual compilation commands
- Adding new tests
- Test framework documentation
