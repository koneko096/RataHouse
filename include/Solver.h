#ifndef SOLVER_H
#define SOLVER_H

#include <vector>
#include <string>
#include "Interval.h"
#include "Device.h"

// Global Variables (External Declarations)
extern char tmp[1024];
extern int totalSlot, ninterval, ProLevel, powerLimit, ndevice;
extern std::vector<Interval> intervals;
extern std::vector<std::vector<int>> proCost;
extern std::vector<int> ProLimit;
extern std::vector<Device> devices;
extern std::vector<double> mean;
extern char* input;

// Suggestion for an optional device that couldn't be scheduled
struct DeviceSuggestion {
    std::string deviceName;
    int power;
    std::string reason;          // why it failed
    std::string recommendation;  // actionable advice
};

// Extended solve result with suggestions
struct SolveResult {
    bool solvable;
    int totalCost;
    std::vector<DeviceSuggestion> suggestions;
};

// Function Prototypes
void FileInput();
void YamlFileInput();
int slotCost(int curSlot, int power);
int GetCost();
bool set(Device& d);
void calculateMean();
void sortDevices();
bool Solve();
SolveResult SolveWithSuggestions();
DeviceSuggestion DiagnoseDevice(const Device& d);

#endif // SOLVER_H
