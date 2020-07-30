// Use the Zumo's gyrometer to measure how much the robot
// has turned around its Z-axis.

#include <Zumo32U4.h>
#include <TurnSensor.hpp>

namespace {
  // The L3GD20H gyrometer sensor in the Zumo
  L3G gyro = L3G{};

  // LCD for messages
  Zumo32U4LCD lcd = Zumo32U4LCD{};

  // Interface with and update/track the gyro
  TurnSensor sensor = TurnSensor{gyro, lcd};
} 

void setup()
{
  sensor.calibrate();
  sensor.reset();
  lcd.clear();
}

void loop()
{
  // Update the gyro value. Should be called as frequently 
  // as possible while using the gyro to measure turns.
  sensor.update();

  // Read current gyro Z-axis estimation
  int32_t angle = sensor.getAngle();

  // Display Z-axis position
  lcd.gotoXY(0, 0);
  lcd.print(angle);
  lcd.print(" ");
}
