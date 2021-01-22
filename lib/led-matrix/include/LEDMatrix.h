/* LED Matrix Header */
#pragma once
#include "hw_bridge.h"
class LEDMatrix {
 public:
  // Constructor: An LED Matrix is defined by the pins it is connected to.
  LEDMatrix(PinName R, PinName G, PinName B);

  // Take in values from 0 to 255 for each pin and map them to a PWM signal.
  void setColor(uint8_t R, uint8_t G, uint8_t B);

  // Just for convenience. Call setColorRGB().
  void setColor(HWBRIDGE::LEDMATRIX::color c);

  // Set the specified color, wait, turn off all LEDs, wait
  // Repeat until another function is called by main.cpp.
  void setFlashingColor(uint8_t R, uint8_t G, uint8_t B, float period_sec);

  // Just for convenience. Call setFlashingColorRGB().
  void setFlashingColor(HWBRIDGE::LEDMATRIX::color c, float period_sec);

  // Turn off all the LEDs. Call setColor(0,0,0).
  void clear();

 private:
  PwmOut m_RChannel;
  PwmOut m_GChannel;
  PwmOut m_BChannel;
  // any other functions
};
