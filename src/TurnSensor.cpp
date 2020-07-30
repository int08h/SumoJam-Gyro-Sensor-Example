/* 
 * Configure the L3GD20H gyro, calibrating it, and use it to
 * measure how much the robot has turned about its Z axis. 
 * 
 * Adapted from https://github.com/DavidEGrayson/zumo-red
 */

#include <Wire.h>
#include "TurnSensor.hpp"

TurnSensor::TurnSensor(L3G &gyro, Zumo32U4LCD &lcd)
    : gyro(gyro),
      lcd(lcd),
      turnAngle(0),
      turnRate(0),
      gyroOffset(0),
      gyroLastUpdate(0)
{
}

void TurnSensor::calibrate()
{
    Wire.begin();
    gyro.init();

    // 800 Hz output data rate,
    // low-pass filter cutoff 100 Hz
    gyro.writeReg(L3G::CTRL1, 0b11111111);

    // 2000 dps full scale
    gyro.writeReg(L3G::CTRL4, 0b00100000);

    // High-pass filter disabled
    gyro.writeReg(L3G::CTRL5, 0b00000000);

    lcd.clear();
    lcd.print(F("Gyro cal"));

    // Turn on the yellow LED in case the LCD is not available.
    ledYellow(1);

    // Delay to give the user time to remove their finger.
    delay(500);

    // Calibrate the gyro.
    int32_t total = 0;
    for (uint16_t i = 0; i < 1024; i++)
    {
        // Wait for new data to be available, then read it.
        while (!(gyro.readReg(L3G::STATUS_REG) & 0x08)) {
            // loop 
        }
        gyro.read();

        // Add the Z axis reading to the total.
        total += gyro.g.z;
    }
    ledYellow(0);
    gyroOffset = total / 1024;
}

void TurnSensor::reset()
{
    gyroLastUpdate = micros();
    turnAngle = 0;
}

void TurnSensor::update()
{
    // Read the measurements from the gyro.
    gyro.read();
    turnRate = gyro.g.z - gyroOffset;

    // Figure out how much time has passed since the last update (dt)
    uint16_t m = micros();
    uint16_t dt = m - gyroLastUpdate;
    gyroLastUpdate = m;

    // Multiply dt by turnRate in order to get an estimation of how
    // much the robot has turned since the last update.
    // (angular change = angular velocity * time)
    int32_t d = (int32_t)turnRate * dt;

    // The units of d are gyro digits times microseconds.  We need
    // to convert those to the units of turnAngle, where 2^29 units
    // represents 45 degrees.  The conversion from gyro digits to
    // degrees per second (dps) is determined by the sensitivity of
    // the gyro: 0.07 degrees per second per digit.
    //
    // (0.07 dps/digit) * (1/1000000 s/us) * (2^29/45 unit/degree)
    // = 14680064/17578125 unit/(digit*us)
    turnAngle += (int64_t)d * 14680064 / 17578125;
}

int32_t TurnSensor::getAngle() {
  // This fancy math converts the number into degrees turned since the
  // last sensor reset.
  return (((int32_t)turnAngle >> 16) * 360) >> 16;
}