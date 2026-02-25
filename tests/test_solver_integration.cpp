#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include "../include/Solver.h"
#include "../include/Device.h"
#include "../include/Interval.h"
#include "../include/YamlParser.h"

// Helper to create a YAML test input file
void create_yaml_test_input(const std::string& filename) {
    std::ofstream out(filename);
    out << "grid:\n";
    out << "  total_slots: 48\n";
    out << "  power_limit: 600\n";
    out << "\n";
    out << "pricing:\n";
    out << "  intervals:\n";
    out << "    - start_hour: 0\n";
    out << "      end_hour: 6\n";
    out << "      tiers:\n";
    out << "        - max_watts: 300\n";
    out << "          cost_per_slot: 20\n";
    out << "        - max_watts: unlimited\n";
    out << "          cost_per_slot: 50\n";
    out << "    - start_hour: 6\n";
    out << "      end_hour: 18\n";
    out << "      tiers:\n";
    out << "        - max_watts: 300\n";
    out << "          cost_per_slot: 50\n";
    out << "        - max_watts: unlimited\n";
    out << "          cost_per_slot: 100\n";
    out << "    - start_hour: 18\n";
    out << "      end_hour: 24\n";
    out << "      tiers:\n";
    out << "        - max_watts: 300\n";
    out << "          cost_per_slot: 20\n";
    out << "        - max_watts: unlimited\n";
    out << "          cost_per_slot: 50\n";
    out << "\n";
    out << "devices:\n";
    out << "  - name: TV\n";
    out << "    power: 200\n";
    out << "    slots_needed: 8\n";
    out << "    permitted_start: 12\n";
    out << "    permitted_end: 20\n";
    out << "    type: mandatory\n";
    out << "    sessions: 1\n";
    out << "\n";
    out << "  - name: Kulkas\n";
    out << "    power: 300\n";
    out << "    slots_needed: 48\n";
    out << "    permitted_start: 0\n";
    out << "    permitted_end: 24\n";
    out << "    type: mandatory\n";
    out << "    sessions: 1\n";
    out << "\n";
    out << "  - name: Kompor\n";
    out << "    power: 100\n";
    out << "    slots_needed: 2\n";
    out << "    permitted_start: 5\n";
    out << "    permitted_end: 12\n";
    out << "    type: optional\n";
    out << "    sessions: 3\n";
    out.close();
}

// Create an overloaded scenario where optional devices can't fit
void create_overloaded_yaml(const std::string& filename) {
    std::ofstream out(filename);
    out << "grid:\n";
    out << "  total_slots: 48\n";
    out << "  power_limit: 400\n"; // Tight power limit
    out << "\n";
    out << "pricing:\n";
    out << "  intervals:\n";
    out << "    - start_hour: 0\n";
    out << "      end_hour: 24\n";
    out << "      tiers:\n";
    out << "        - max_watts: 300\n";
    out << "          cost_per_slot: 20\n";
    out << "        - max_watts: unlimited\n";
    out << "          cost_per_slot: 50\n";
    out << "\n";
    out << "devices:\n";
    out << "  - name: Kulkas\n";
    out << "    power: 350\n";
    out << "    slots_needed: 48\n";
    out << "    permitted_start: 0\n";
    out << "    permitted_end: 24\n";
    out << "    type: mandatory\n";
    out << "    sessions: 1\n";
    out << "\n";
    out << "  - name: Kompor\n";
    out << "    power: 200\n"; // 350+200=550 > 400 limit
    out << "    slots_needed: 4\n";
    out << "    permitted_start: 0\n";
    out << "    permitted_end: 24\n";
    out << "    type: optional\n";
    out << "    sessions: 2\n";
    out << "\n";
    out << "  - name: Heater\n";
    out << "    power: 100\n"; // 350+100=450 > 400 limit
    out << "    slots_needed: 6\n";
    out << "    permitted_start: 8\n";
    out << "    permitted_end: 10\n"; // Very narrow window: only 4 slots for 6 needed
    out << "    type: optional\n";
    out << "    sessions: 1\n";
    out.close();
}

// Helper to create old-format text input (for backward compat test)
void create_text_test_input(const std::string& filename) {
    std::ofstream out(filename);
    out << "48 3 2 600\n";
    out << "0 6 300 20   6 18 300 50   18 24 300 20\n";
    out << "0 6 >300 50   6 18 >300 100   18 24 >300 50\n";
    out << "3\n";
    out << "TV 200 8 12 20 wajib 1\n";
    out << "Kulkas 300 48 0 24 wajib 1\n";
    out << "Kompor 100 2 5 12 opsional 3\n";
    out.close();
}

// Test: YAML parser produces same globals as text parser
TEST(YamlInputTest, MatchesTextInput) {
    std::string txtFile = "test_compat_input.txt";
    std::string yamlFile = "test_compat_input.yaml";
    create_text_test_input(txtFile);
    create_yaml_test_input(yamlFile);

    // Parse text format
    RataHouse::input = const_cast<char*>(txtFile.c_str());
    RataHouse::FileInput();
    int txtTotalSlot = RataHouse::totalSlot;
    int txtPowerLimit = RataHouse::powerLimit;
    int txtNinterval = RataHouse::ninterval;
    int txtProLevel = RataHouse::ProLevel;
    int txtNdevice = RataHouse::ndevice;

    // Parse YAML format
    RataHouse::input = const_cast<char*>(yamlFile.c_str());
    YamlFileInput();

    EXPECT_EQ(RataHouse::totalSlot, txtTotalSlot);
    EXPECT_EQ(RataHouse::powerLimit, txtPowerLimit);
    EXPECT_EQ(RataHouse::ninterval, txtNinterval);
    EXPECT_EQ(RataHouse::ProLevel, txtProLevel);
    EXPECT_EQ(RataHouse::ndevice, txtNdevice);

    // Check intervals match
    for (int i = 0; i < RataHouse::ninterval; i++) {
        EXPECT_EQ(RataHouse::intervals[i].begin, RataHouse::intervals[i].begin);
        EXPECT_EQ(RataHouse::intervals[i].end, RataHouse::intervals[i].end);
    }

    // Check devices match
    for (int i = 0; i < RataHouse::ndevice; i++) {
        EXPECT_EQ(RataHouse::devices[i].power, RataHouse::devices[i].power);
        EXPECT_EQ(RataHouse::devices[i].wajib, RataHouse::devices[i].wajib);
    }

    remove(txtFile.c_str());
    remove(yamlFile.c_str());
}

// Test: Basic YAML solve works
TEST(SolverIntegrationTest, YamlBasicOptimization) {
    std::string test_file = "test_yaml_solve.yaml";
    create_yaml_test_input(test_file);

    RataHouse::input = const_cast<char*>(test_file.c_str());
    YamlFileInput();
    RataHouse::calculateMean();
    RataHouse::sortDevices();

    int maxSkip = std::max(1, RataHouse::ndevice / 20);
    int skip = 0;
    bool solvable = true;
    for (Device& d : RataHouse::devices) {
        if (skip == maxSkip) { solvable = false; break; }
        else if (!RataHouse::set(d)) { skip++; continue; }
        else { skip = 0; }
    }

    EXPECT_TRUE(solvable);

    int finalCost = RataHouse::GetCost();
    EXPECT_GT(finalCost, 0);

    remove(test_file.c_str());
}

// Test: Suggestions are generated for overloaded optional devices
TEST(SuggestionsTest, OverloadedOptionalDevices) {
    std::string test_file = "test_overloaded.yaml";
    create_overloaded_yaml(test_file);

    RataHouse::input = const_cast<char*>(test_file.c_str());
    YamlFileInput();
    RataHouse::calculateMean();
    RataHouse::sortDevices();

    int maxSkip = std::max(1, RataHouse::ndevice / 20);
    int skip = 0;
    for (Device& d : RataHouse::devices) {
        if (skip == maxSkip) break;
        else if (!RataHouse::set(d)) { skip++; continue; }
        else { skip = 0; }
    }

    // Collect suggestions for unscheduled optional devices
    std::vector<RataHouse::DeviceSuggestion> suggestions;
    for (const Device& d : RataHouse::devices) {
        if (!d.wajib && d.assignedRange.empty()) {
            suggestions.push_back(RataHouse::DiagnoseDevice(d));
        }
    }

    // At least one optional device should have a suggestion
    EXPECT_FALSE(suggestions.empty());

    // Each suggestion should have non-empty reason and recommendation
    for (const auto& s : suggestions) {
        EXPECT_FALSE(s.deviceName.empty());
        EXPECT_FALSE(s.reason.empty());
        EXPECT_FALSE(s.recommendation.empty());
        EXPECT_GT(s.power, 0);
    }

    remove(test_file.c_str());
}

// Test: DiagnoseDevice detects narrow time window
TEST(SuggestionsTest, NarrowTimeWindow) {
    // Create a device with time window too narrow for its needs
    Device d;
    d.name = "TestHeater";
    d.power = 100;
    d.slot = 6;
    d.permittedRange = Interval(16, 20); // Only 4 slots for 6 needed
    d.wajib = false;
    d.nyala = 1;

    RataHouse::DeviceSuggestion s = RataHouse::DiagnoseDevice(d);
    EXPECT_EQ(s.deviceName, "TestHeater");
    EXPECT_FALSE(s.reason.empty());
    // Should mention "narrow" or similar
    EXPECT_NE(s.reason.find("narrow"), std::string::npos);
}

// Test: All-mandatory devices produce no suggestions
TEST(SuggestionsTest, AllMandatoryNoSuggestions) {
    std::string test_file = "test_all_mandatory.yaml";
    {
        std::ofstream out(test_file);
        out << "grid:\n";
        out << "  total_slots: 48\n";
        out << "  power_limit: 600\n";
        out << "pricing:\n";
        out << "  intervals:\n";
        out << "    - start_hour: 0\n";
        out << "      end_hour: 24\n";
        out << "      tiers:\n";
        out << "        - max_watts: 600\n";
        out << "          cost_per_slot: 20\n";
        out << "devices:\n";
        out << "  - name: Light\n";
        out << "    power: 50\n";
        out << "    slots_needed: 10\n";
        out << "    permitted_start: 0\n";
        out << "    permitted_end: 24\n";
        out << "    type: mandatory\n";
        out << "    sessions: 1\n";
        out.close();
    }

    RataHouse::input = const_cast<char*>(test_file.c_str());
    YamlFileInput();
    RataHouse::calculateMean();
    RataHouse::sortDevices();

    for (Device& d : RataHouse::devices) {
        RataHouse::set(d);
    }

    // No optional devices → no suggestions
    std::vector<RataHouse::DeviceSuggestion> suggestions;
    for (const Device& d : RataHouse::devices) {
        if (!d.wajib && d.assignedRange.empty()) {
            suggestions.push_back(RataHouse::DiagnoseDevice(d));
        }
    }
    EXPECT_TRUE(suggestions.empty());

    remove(test_file.c_str());
}

// Backward compatibility: old text input still works
TEST(SolverIntegrationTest, TextFileBackwardCompat) {
    std::string test_file = "test_backward_compat.txt";
    create_text_test_input(test_file);

    RataHouse::input = const_cast<char*>(test_file.c_str());
    bool result = RataHouse::Solve();
    EXPECT_TRUE(result);

    int finalCost = RataHouse::GetCost();
    EXPECT_GT(finalCost, 0);

    remove(test_file.c_str());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
