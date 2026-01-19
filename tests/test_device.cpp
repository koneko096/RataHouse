#include <gtest/gtest.h>
#include "../include/Device.h"
#include "../include/Interval.h"

// Test Device Constructor
TEST(DeviceTest, Constructor) {
    Device dev;
    EXPECT_EQ(dev.value, 0.0);
}

// Test Device IsWajib
TEST(DeviceTest, IsWajib) {
    Device dev1;
    dev1.wajib = true;
    EXPECT_TRUE(dev1.IsWajib());
    
    Device dev2;
    dev2.wajib = false;
    EXPECT_FALSE(dev2.IsWajib());
}

// Test Device IsPermitted - Valid Case
TEST(DeviceTest, IsPermittedValid) {
    Device dev;
    dev.permittedRange = Interval(0, 48);
    dev.nyala = 2;
    
    // Add two valid assigned ranges
    dev.assignedRange.push_back(Interval(5, 10));
    dev.assignedRange.push_back(Interval(20, 25));
    
    EXPECT_TRUE(dev.IsPermitted());
}

// Test Device IsPermitted - Wrong Count
TEST(DeviceTest, IsPermittedWrongCount) {
    Device dev;
    dev.permittedRange = Interval(0, 48);
    dev.nyala = 2;
    
    // Add only one range when nyala=2
    dev.assignedRange.push_back(Interval(5, 10));
    
    EXPECT_FALSE(dev.IsPermitted());
}

// Test Device IsPermitted - Out of Range
TEST(DeviceTest, IsPermittedOutOfRange) {
    Device dev;
    dev.permittedRange = Interval(10, 30);
    dev.nyala = 1;
    
    // Add range outside permitted range
    dev.assignedRange.push_back(Interval(5, 15)); // begins before permitted range
    
    EXPECT_FALSE(dev.IsPermitted());
}

// Test Device Comparison Operator
TEST(DeviceTest, Comparison) {
    Device dev1, dev2;
    
    // Test wajib priority
    dev1.wajib = true;
    dev1.value = 100.0;
    dev2.wajib = false;
    dev2.value = 50.0;
    
    EXPECT_TRUE(dev1 < dev2); // Wajib devices have higher priority
    
    // Test value comparison when both have same wajib status
    Device dev3, dev4;
    dev3.wajib = false;
    dev3.value = 100.0;
    dev4.wajib = false;
    dev4.value = 200.0;
    
    EXPECT_TRUE(dev3 < dev4); // Lower value has higher priority
}

// Test Device Properties
TEST(DeviceTest, Properties) {
    Device dev;
    dev.name = "TestDevice";
    dev.power = 500;
    dev.slot = 10;
    dev.nyala = 2;
    
    EXPECT_EQ(dev.name, "TestDevice");
    EXPECT_EQ(dev.power, 500);
    EXPECT_EQ(dev.slot, 10);
    EXPECT_EQ(dev.nyala, 2);
}
