/* 
 * Configure the L3GD20H gyro, calibrating it, and use it to
 * measure how much the robot has turned about its Z axis. 
 * 
 * Adapted from https://github.com/DavidEGrayson/zumo-red
 */

#pragma once

#include <Zumo32U4.h>

// Interface with the L3GD20H on-board gyrometer in the Zumo32U4
class TurnSensor
{
public:
    // Create a new turn sensor instance
    TurnSensor(L3G &gyro, Zumo32U4LCD &lcd);

    TurnSensor() = delete;
    TurnSensor& operator=(const TurnSensor&) = delete;

    // This should be called in setup() to enable and calibrate the
    // gyro. It uses the LCD, yellow LED, and button A. While the LCD
    // is displaying "Gyro cal", you should be careful to hold the robot
    // still.
    //
    // The digital zero-rate level of the L3GD20H gyro can be as high as
    // 25 degrees per second, and this calibration helps us correct for
    // that.
    void calibrate();

    // This should be called to set the starting point for measuring
    // a turn. After calling this, turnAngle will be 0.
    void reset();

    // Read the gyro and update the angle. This should be called as
    // frequently as possible while using the gyro to do turns.
    void update();

    // Return degrees turned since the last sensor reset.
    int32_t getAngle(); 

private:
    // L3GD20H gyro
    L3G& gyro;

    // LCD for displaying messages
    Zumo32U4LCD& lcd;

    // turnAngle is a 32-bit unsigned integer representing the amount
    // the robot has turned since the last time sensorReset() was
    // called. This is computed solely using the Z axis of the gyro, so
    // it could be inaccurate if the robot is rotated about the X or Y
    // axes.
    //
    // Our convention is that a value of 0x20000000 represents a 45
    // degree counter-clockwise rotation. This means that a uint32_t
    // can represent any angle between 0 degrees and 360 degrees. If
    // you cast it to a signed 32-bit integer by writing
    // (int32_t)turnAngle, that integer can represent any angle between
    // -180 degrees and 180 degrees. 
    uint32_t turnAngle;

    // turnRate is the current angular rate of the gyro, in units of
    // 0.07 degrees per second.
    int16_t turnRate;

    // This is the average reading obtained from the gyro's Z axis
    // during calibration.
    int16_t gyroOffset;

    // This variable helps us keep track of how much time has passed
    // between readings of the gyro.
    uint16_t gyroLastUpdate;
};
