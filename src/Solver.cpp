#include "Solver.h"
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <vector>

// Global Variable Definitions
char tmp[1024];
int totalSlot, ninterval, ProLevel, powerLimit, ndevice;
std::vector<Interval> intervals;
std::vector<std::vector<int>> proCost;
std::vector<int> ProLimit;
std::vector<Device> devices;
std::vector<double> mean;
char* input;

using namespace std;

// inputing all variables from chosen input file
void FileInput()
{
	FILE* fi = fopen(input, "r");

	fscanf(fi, "%d", &totalSlot);
	fscanf(fi, "%d", &ninterval);
	fscanf(fi, "%d", &ProLevel);
	fscanf(fi, "%d", &powerLimit);

	intervals.clear();
	ProLimit.clear();
	proCost.clear();
	devices.clear();

	intervals.resize(ninterval + 2);
	ProLimit.resize(ProLevel);
	proCost.resize(ProLevel);

	for (int i = 0; i < ProLevel; ++i)
	{
		proCost[i].resize(ninterval);
		for (int j = 0; j < ninterval; ++j)
		{
			// Only read intervals once (on first pricing level)
			if (i == 0)
			{
				fscanf(fi, "%d", &intervals[j].begin);
				fscanf(fi, "%d", &intervals[j].end);
			}
			else
			{
				// Skip interval data for subsequent pricing levels
				int dummy;
				fscanf(fi, "%d", &dummy);
				fscanf(fi, "%d", &dummy);
			}
			
			fscanf(fi, "%s", tmp);

			if (!j)
				ProLimit[i] = (tmp[0] != '>' ? atoi(tmp) : 999999);

			fscanf(fi, "%d", &proCost[i][j]);
		}
	}

	fscanf(fi, "%d", &ndevice);
	devices.resize(ndevice);
	for (Device& d : devices)
	{
		fscanf(fi, "%s", tmp);
		d.name = string(tmp);
		fscanf(fi, "%d", &d.power);
		fscanf(fi, "%d", &d.slot);
		fscanf(fi, "%d", &d.permittedRange.begin);
		d.permittedRange.begin *= 2;
		fscanf(fi, "%d", &d.permittedRange.end);
		d.permittedRange.end *= 2;
		fscanf(fi, "%s", tmp);
		d.wajib = (string(tmp) == string("wajib"));
		fscanf(fi, "%d", &d.nyala);
	}

	fclose(fi);
}

// find cost of using certain slot
int slotCost(int curSlot, int power)
{
	// Find which interval this slot belongs to
	int curIntv = -1;
	for (int i = 0; i < ninterval; ++i)
	{
		if (curSlot >= intervals[i].begin && curSlot < intervals[i].end)
		{
			curIntv = i;
			break;
		}
	}
	
	if (curIntv == -1) return 0; // Slot not in any interval
	
	// Find which pricing level based on power consumption
	int priceLevel = 0;
	for (int i = 0; i < ProLevel; ++i)
	{
		if (power <= ProLimit[i])
		{
			priceLevel = i;
			break;
		}
	}
	
	return proCost[priceLevel][curIntv];
}

// compute total cost of current configuration
// return -1 if answer doesnt exist
// and return actual total cost
int GetCost()
{
	int power[50] = {0};
	int result = 0;

	for (Device dev : devices)
	{
		// Fix: Skip devices that haven't been assigned a range yet (during sequential solving)
		if (dev.assignedRange.empty()) continue;

		if (!dev.IsPermitted()) return -1;

		for (Interval& rng : dev.assignedRange)
		{
			power[rng.begin] += dev.power;
			power[rng.end] -= dev.power;
		}
	}

	if (power[0] > powerLimit) return -1;
	for (int i = 1; i < 48; ++i)
	{
		power[i] += power[i - 1];
		if (power[i] > powerLimit) return -1;
	}

	for (int i = 0; i < 48; ++i)
	{
		result += slotCost(i, power[i]);
	}

	return result;
}

// set posisi tiap range
bool set(Device& d)
{
	d.assignedRange.clear();
	if (d.permittedRange.end - d.permittedRange.begin < d.slot * d.nyala)
		return false;

	for (int i = 0; i < d.nyala; ++i)
	{
		d.assignedRange.push_back(
			Interval(d.permittedRange.begin + i * d.slot,
				d.permittedRange.begin + (i + 1) * d.slot)
		);
	}

	bool result = true;
	int ujung = d.permittedRange.end;
	for (int i = d.assignedRange.size() - 1; i >= 0; --i)
	{
		double const_sekre = 0.37;	//Secretary Theorem
		Interval& curRange = d.assignedRange[i];
		// Fix: Store best interval to restore if no better option found or loop exhaust
		Interval bestInterval = curRange;
		int const_limit = (ujung - curRange.end) * const_sekre;
		int mini_cost;
		bool vavalita;

		mini_cost = GetCost();
		for (int j = 0; j<const_limit; j++) {
			++curRange;
			int temp_cost = GetCost(); 
			if (temp_cost> -1) {
				if (temp_cost<mini_cost) {
					mini_cost = temp_cost;
					bestInterval = curRange; // Track best in sample
				}
			}
		}
		int j = const_limit;
		bool ketemu = false;
		while (!ketemu && j<ujung) {
			++curRange;
			int temp_cost = GetCost(); 
			if (temp_cost> -1) {
				if (temp_cost <= mini_cost) {
					ketemu = true;
					mini_cost = temp_cost;
					// curRange is now at the accepted position
				}
			}
			if (j == ujung - 1 && !ketemu) {
				// Last slot check logic (existing)? 
				// The original code was: 'ketemu = true; mini_cost = temp_cost;'
				// But temp_cost might be -1? 
				// Let's rely on the restoration logic below.
			}
			j++;
		}
		
		if (!ketemu) {
			// If we didn't find a stopping candidate, revert to the best one found in/before sample
			curRange = bestInterval;
		}

		// Re-check cost to ensure valid result
		if (GetCost() == -1) {
			result = false;
		}
		else
		{
			result = true;
		}
	}
	return result;
}

// calculate mean of progressive graphics
// heuristic approach
void calculateMean()
{
	std::vector< std::vector<int> > vero;
	vero.resize(ProLevel);

	for (int i = 0; i < ProLevel; i++)
	{
		vero[i].resize(50);
		for (int j = 0; j < ninterval; j++)
		{
			Interval& tmp = intervals[j];

			vero[i][tmp.begin] += proCost[i][j];
			vero[i][tmp.end] -= proCost[i][j];
		}
	}

	for (int i = 0; i < ProLevel; i++)
	{
		for (int j = 1; j < 48; j++)
		{
			vero[i][j] += vero[i][j - 1];
		}
	}

	mean.resize(49);
	for (int i = 0; i < 48; i++)
	{
		double sum = 0;

		for (int j = 0; j < ProLevel; j++)
		{
			sum += vero[j][i];
		}

		sum /= ProLevel;
		mean[i] = sum;
	}
}

// sorting devices according to priority value
// as explained in docs
void sortDevices()
{
	calculateMean();

	for (Device& dev : devices)
	{
		for (Interval& rng : dev.assignedRange)
			for (int slotUsed = rng.begin;
				slotUsed < rng.end; ++slotUsed)
		{
			dev.value += mean[slotUsed] * slotCost(slotUsed, dev.power);
		}
		dev.value *= dev.power * dev.nyala;
		dev.value /= dev.slot;
	}

	sort(devices.begin(), devices.end());
}

// main solver :
// returning answer availability (exist / not)
bool Solve()
{
	FileInput();
	calculateMean();
	sortDevices();

	int maxSkip = max(1, ndevice / 20);
	int skip = 0;
	for (Device& d : devices)
	{
		if (skip == maxSkip)
		{
			return false;
		}
		else if (set(d) == false)
		{
			skip++;
			continue;
		}
		else
		{
			skip = 0;
		}
	}

	return true;
}
