#include "gtest/gtest.h"
#include "etc_controller.h"

// Test fixture class for ETCController
class TestETCController : public ::testing::Test {
protected:
    ETCController controller;

    void SetUp() override {
        // Perform any necessary setup before each test
        controller.resetState();
    }

    void TearDown() override {
        // Clean up after each test if needed
    }
};

// Test Constructor and Initial State
TEST_F(TestETCController, ConstructorInitializesCorrectly) {
    ETCState state = controller.getState();

    EXPECT_EQ(state.mbb_alive, 0);
    EXPECT_FLOAT_EQ(state.he1_read, 0.f);
    EXPECT_FLOAT_EQ(state.he2_read, 0.f);
    EXPECT_FLOAT_EQ(state.he1_travel, 0.f);
    EXPECT_FLOAT_EQ(state.he2_travel, 0.f);
    EXPECT_FLOAT_EQ(state.pedal_travel, 0.f);
    EXPECT_FLOAT_EQ(state.brakes_read, 0.f);
    EXPECT_FALSE(state.ts_ready);
    EXPECT_FALSE(state.motor_enabled);
    EXPECT_TRUE(state.motor_forward);
    EXPECT_FALSE(state.cockpit);
    EXPECT_EQ(state.torque_demand, 0);
}

// Test resetState
TEST_F(TestETCController, ResetStateWorksCorrectly) {
    // Modify the state
    controller.updateMBBAlive();
    controller.updateStateFromCAN({15, 2.1f, 2.2f, 0.5f, 0.6f, 1.0f, 0.8f, true, true, false, true, 10000});

    // Reset state
    controller.resetState();
    ETCState state = controller.getState();

    // Ensure state is reset correctly
    EXPECT_EQ(state.mbb_alive, 0);
    EXPECT_FLOAT_EQ(state.he1_read, 0.f);
    EXPECT_FLOAT_EQ(state.he2_read, 0.f);
    EXPECT_FLOAT_EQ(state.he1_travel, 0.f);
    EXPECT_FLOAT_EQ(state.he2_travel, 0.f);
    EXPECT_FLOAT_EQ(state.pedal_travel, 0.f);
    EXPECT_FLOAT_EQ(state.brakes_read, 0.f);
    EXPECT_FALSE(state.ts_ready);
    EXPECT_FALSE(state.motor_enabled);
    EXPECT_TRUE(state.motor_forward);
    EXPECT_FALSE(state.cockpit);
    EXPECT_EQ(state.torque_demand, 0);
}

// Test updateMBBAlive
TEST_F(TestETCController, UpdateMBBAliveIncrementsCorrectly) {
    for (int i = 0; i < 20; ++i) {
        controller.updateMBBAlive();
    }
    EXPECT_EQ(controller.getMBBAlive(), 4); // 20 % 16 = 4
}

// Test updateStateFromCAN
TEST_F(TestETCController, UpdateStateFromCANWorksCorrectly) {
    ETCState new_state = {15, 1.5f, 2.0f, 0.8f, 0.7f, 0.9f, 0.4f, true, false, true, false, 20000};
    controller.updateStateFromCAN(new_state);

    ETCState state = controller.getState();
    EXPECT_EQ(state.mbb_alive, 15);
    EXPECT_FLOAT_EQ(state.he1_read, 1.5f);
    EXPECT_FLOAT_EQ(state.he2_read, 2.0f);
    EXPECT_FLOAT_EQ(state.he1_travel, 0.8f);
    EXPECT_FLOAT_EQ(state.he2_travel, 0.7f);
    EXPECT_FLOAT_EQ(state.pedal_travel, 0.9f);
    EXPECT_FLOAT_EQ(state.brakes_read, 0.4f);
    EXPECT_TRUE(state.ts_ready);
    EXPECT_FALSE(state.motor_enabled);
    EXPECT_TRUE(state.motor_forward);
    EXPECT_FALSE(state.cockpit);
    EXPECT_EQ(state.torque_demand, 20000);
}

// Test switchMotorDirection
TEST_F(TestETCController, SwitchMotorDirectionWorks) {
    controller.switchReverseMotor();
    EXPECT_FALSE(controller.isMotorForward());

    controller.switchForwardMotor();
    EXPECT_TRUE(controller.isMotorForward());
}

// Test turnOffMotor
TEST_F(TestETCController, TurnOffMotorDisablesMotor) {
    controller.turnOffMotor();
    EXPECT_FALSE(controller.isMotorEnabled());
}

// TODO: Add tests for updatePedalTravel, checkStartConditions, and runRTDS when implemented

