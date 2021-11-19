#include "mbed.h"
#include "TutorialServo.h"

// Initialize member variables and servo
TutorialServo::TutorialServo(PinName servoPin, float servoRangeInDegrees, 
                             float minPulsewidthInMs, float maxPulsewidthInMs):
m_servoPwmOut(servoPin),
m_servoRangeInDegrees(servoRangeInDegrees),
m_minPulsewidthInMs(minPulsewidthInMs),
m_maxPulsewidthInMs(maxPulsewidthInMs)
{
    // Set servo angle to the middle point
    m_servoPwmOut.pulsewidth(
        (m_minPulsewidthInMs + m_maxPulsewidthInMs) / 2000
    );
}

void TutorialServo::setPositionInDegrees(const float degrees) {
    float pulseWidth{};
    
    if (degrees <= 0) {
        pulseWidth = getMinPulseWidthInMs() / 1000;
    } else if (degrees < m_servoRangeInDegrees) {
        pulseWidth = (
            getMinPulseWidthInMs() 
          + (getMaxPulseWidthInMs() - getMinPulseWidthInMs())
          * degrees/getServoRangeInDegrees()
        ) / 1000;
    } else {
        pulseWidth = getMaxPulseWidthInMs() / 1000;
    }

    m_servoPwmOut.pulsewidth(pulseWidth);
}

float TutorialServo::getServoRangeInDegrees() const {
    return m_servoRangeInDegrees;
}

float TutorialServo::getMinPulseWidthInMs() const {
    return m_minPulsewidthInMs;
}

float TutorialServo::getMaxPulseWidthInMs() const {
    return m_maxPulsewidthInMs;
}