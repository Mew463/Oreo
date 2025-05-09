#include <Arduino.h>

struct PID {
  float kP = .01;
  float kI = 0;
  float kD = .01;
  float maxError = 100;
  float maxI = 2;
  float maxOut = 2;
} PID;

int calcTurnPower(float headingError) {
  if (headingError > PID.maxError)
    headingError = PID.maxError;
  if (headingError < -PID.maxError)
    headingError = -PID.maxError;
  // Static variables to store the previous error and the integral of errors
  static float prevHeadingError = 0;
  static float integral = 0;
  static unsigned long lastCalc = millis();
  float derivative;
  float output;

  // PID calculations
  integral += headingError;
  if (integral > PID.maxI)
    integral = PID.maxI;
  if ((headingError > 0 && prevHeadingError < 0) || (headingError < 0 && prevHeadingError > 0)) 
    integral = 0;

  derivative = headingError - prevHeadingError / (millis() - lastCalc);
  lastCalc = millis();
  // PID output calculation
  output = PID.kP * headingError + PID.kI * integral + PID.kD * derivative;

  if (output > PID.maxOut)
    output = PID.maxOut;
  else if(output < -PID.maxOut)
    output = -PID.maxOut;

  // Update previous error
  prevHeadingError = headingError;

  return (int)output;
}