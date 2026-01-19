#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include "../include/Solver.h"
#include "../include/Device.h"
#include "../include/Interval.h"

// Helper to create a temp file
void create_test_input(const std::string& filename) {
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

// Test Solver Integration
TEST(SolverIntegrationTest, BasicOptimization) {
    std::string test_file = "integration_test_input.txt";
    create_test_input(test_file);

    // Setup global 'input' variable required by Solver logic
    input = const_cast<char*>(test_file.c_str());

    bool result = Solve();
    EXPECT_TRUE(result);

    int finalCost = GetCost();
    EXPECT_GT(finalCost, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
