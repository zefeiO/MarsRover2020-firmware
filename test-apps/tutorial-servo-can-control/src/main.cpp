#include "CANConfig.h"
#include "CANInterface.h"
#include "hw_bridge.h"
#include "TutorialServo.h"
#include "mbed.h"

float percentageToDegrees(float percentage, float maxAngle) {
    return maxAngle * percentage / 100;
}

CANInterface can(CANConfig::config);

int main() {
    HWBRIDGE::CANSignalValue_t servoRangePercentage = 0;
    TutorialServo servo(PA_1);

    // can.setFilter(HWBRIDGE::CANFILTER::ARM_RX_FILTER, CANStandard, HWBRIDGE::ROVER_CANID_FILTER_MASK, 0);
    // can.setFilter(HWBRIDGE::CANFILTER::COMMON_FILTER, CANStandard, HWBRIDGE::ROVER_CANID_FILTER_MASK, 1);

    can.setFilter(HWBRIDGE::CANFILTER::COMMON_FILTER, CANStandard, HWBRIDGE::ROVER_CANID_FILTER_MASK);

    while(true) {
        can.getRXSignalValue(HWBRIDGE::CANID::ARM_SET_JOINT_POSITION, HWBRIDGE::CANSIGNAL::ARM_SET_TURNTABLE_POSITION,
                             servoRangePercentage);
        servo.setPositionInDegrees(percentageToDegrees(servoRangePercentage, servo.getServoRangeInDegrees()));

        ThisThread::sleep_for(1ms);
    }
    
    return 0;
}


