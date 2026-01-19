#include <gtest/gtest.h>
#include "../include/Interval.h"

// Test Interval Constructor
TEST(IntervalTest, Constructor) {
    Interval intv(5, 10);
    EXPECT_EQ(intv.begin, 5);
    EXPECT_EQ(intv.end, 10);
}

// Test IsInside
TEST(IntervalTest, IsInside) {
    Interval intv(5, 10);
    
    // Test values inside
    EXPECT_TRUE(intv.IsInside(5));
    EXPECT_TRUE(intv.IsInside(7));
    EXPECT_TRUE(intv.IsInside(9));
    
    // Test boundary
    EXPECT_FALSE(intv.IsInside(10)); // end is exclusive
    
    // Test values outside
    EXPECT_FALSE(intv.IsInside(4));
    EXPECT_FALSE(intv.IsInside(11));
}

// Test IsCollide
TEST(IntervalTest, IsCollide) {
    Interval intv1(5, 10);
    Interval intv2(8, 12);  // Overlaps with intv1
    Interval intv3(10, 15); // Adjacent, no overlap
    Interval intv4(0, 3);   // Completely separate
    
    EXPECT_TRUE(intv1.IsCollide(intv2));
    EXPECT_FALSE(intv1.IsCollide(intv3));
    EXPECT_FALSE(intv1.IsCollide(intv4));
}

// Test Increment Operator
TEST(IntervalTest, Increment) {
    Interval intv(5, 10);
    ++intv;
    
    EXPECT_EQ(intv.begin, 6);
    EXPECT_EQ(intv.end, 11);
}

// Test Decrement Operator
TEST(IntervalTest, Decrement) {
    Interval intv(5, 10);
    --intv;
    
    EXPECT_EQ(intv.begin, 4);
    EXPECT_EQ(intv.end, 9);
}

// Test Comparison Operator
TEST(IntervalTest, Comparison) {
    Interval intv1(5, 10);
    Interval intv2(8, 12);
    
    EXPECT_TRUE(intv1 < intv2);  // 5 < 8
    EXPECT_FALSE(intv2 < intv1); // 8 > 5
}

// Test Edge Cases
TEST(IntervalTest, EdgeCases) {
    // Zero-length interval
    Interval intv1(5, 5);
    EXPECT_FALSE(intv1.IsInside(5)); // begin <= cur < end, so 5 is not inside [5,5)
    
    // Single slot interval
    Interval intv2(5, 6);
    EXPECT_TRUE(intv2.IsInside(5));
    EXPECT_FALSE(intv2.IsInside(6));
}
