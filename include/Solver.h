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

// Function Prototypes
void FileInput();
int slotCost(int curSlot, int power);
int GetCost();
bool set(Device& d);
void calculateMean();
void sortDevices();
bool Solve();

#endif // SOLVER_H
