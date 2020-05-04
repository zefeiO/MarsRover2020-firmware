#include "mbed.h"
#include "CANMsg.h"
#include "safety.h"
#include "rover_config.h"

// Datasheet for INA226 http://www.ti.com/lit/ds/symlink/ina226.pdf
// current sensor data https://docs.google.com/spreadsheets/d/1Qn4QbEJ1Ia54vYnTgaTvLxHralQvCz6PEGaSF_O1L20/edit#gid=0

//Serial communications for debugging
Serial pc(SERIAL_TX, SERIAL_RX, ROVER_DEFAULT_SERIAL_BAUD_RATE);

//CAN initialization 
CAN can(CAN1_RX, CAN1_TX, ROVER_CANBUS_FREQUENCY); // dont know the actual CAN id's
CANMsg rxMsg;
//needs a specific CANID to update values
const unsigned int EXPECTED_CAN_ID = 0x000;

//measurement thresholds, needs updating
const float CURRENT_UPPER_LIM = 1;
const float CURRENT_LOWER_LIM = 0;
const float CURVOLTAGE_UPPER_LIM = 1;
const float VOLTAGE_LOWER_LIM = 0;
const float TEMP_UPPER_LIM = 1;
const float TEMP_LOWER_LIM = 0;

// temperature conversion constexprants
constexpr float B = 4700; //kelvins. Based on Resistances of 25 and 50 
constexpr float VIN = 5;
//need expected room temp. 22 celcius?
constexpr float ROOM_TEMP = 0;
constexpr float REFERENCE_RESISTANCE = 0;
    

AnalogIn thermistor_voltage_;


void initCAN();
//TODO may need more functions to deal with different CAN message contents
void validateCANMsg(CANMsg *p_newMsg);
int inRange(float upperLim, float lowerLim, float val);

int main()
{
    Safety_Board safetyBoard();

    initCAN();

    // turn led off, turn on if issue occurs
    safetyBoard.led1_ = 0;
    safetyBoard.led2_ = 0;
    safetyBoard.led3_ = 0;

    Safety_Board::calibrateSensor();

    while (true) {
        //read can messages to get data to configure device
        u_int8_t command = 0;
        if(can.read(rxMsg))
        {
            ValidateCANMsg(&rxMsg);
            rxMsg >> command;
            rxMsg.clear();
        }
        //going to need more functionality to select what to measure

        float measuredCurrent = safetyBoard.getCurrentData();
        if (!inRange(CURRENT_UPPER_LIM, CURRENT_LOWER_LIM, measuredCurrent))
        {
            safetyBoard.led1_ = 1;
            pc.printf("Measured Current is NOT within expected limits!\r\n");
        }

        float measuredVoltage = safetyBoard.getVoltageData();
        if (!inRange(CURVOLTAGE_UPPER_LIM, VOLTAGE_LOWER_LIM, measuredVoltage))
        {
            safetyBoard.led2_ = 1;
            pc.printf("Measured Voltage is NOT within expected limits!\r\n");
        }

        float vout = safetyBoard.thermistorvoltage.read();
        float measuredTemp = safetyBoard.readThermistor(safetyBoard.VIN, vout);
        if(!inRange(TEMP_UPPER_LIM, TEMP_LOWER_LIM, measuredTemp))
        {
            safetyBoard.led3_ = 1;
            pc.printf("Measured Temperature is NOT within expected limits!\r\n");
        }

        //shutoff all LED's for next round of checks after 2 seconds
        wait(2);
        safetyBoard.led1_ = 0;
        safetyBoard.led2_ = 0;
        safetyBoard.led3_ = 0;
    }
}

void initCAN()
{
    // will need new id's 
    can.filter(CAN1_RX, CAN1_TX, ROVER_CANID_MASK, CANStandard);
}

void validateCANMsg(CANMsg *p_newMsg)
{
    // PRINT_INFO("Recieved CAN message with ID %X\r\n", p_newMsg->id);
    // The specific can ID for configuring chip is ___
    switch (p_newMsg->id){
        case EXPECTED_CAN_ID:
            pc.printf("Updating INA226 config\r\n");
            break;
        default:
            pc.printf("Recieved unimplemented command\r\n");
            break;
    }
}

//if in range returns 1, otherwise returns 0
int inRange(float upperLim, float lowerLim, float val)
{
    int inRange = (val > upperLim || val< lowerLim) ? 0: 1;
    return inRange;
}

float Safety_Board::readThermistor(float vin, float vout)
{
    pc.printf("Retrieving Temperature of Board\r\n");

    //voltage divider to find resistance of thermister
    //TODO determine what rResistor is
    float thermisterResistance = rResistor *((vin/vout)-1);

    /*  convert resistance to temp (steinhart and hart eqn)
        A,B,C are Stienhart-hart coefficients which depend on the thermistor
        (https://industrial.panasonic.com/cdbs/www-data/pdf/AUA0000/AUA0000C8.pdf)
        and the temperature range of interest. 
        https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation 
        Need to test the thermistor for resistance at 3 different temperatures and use 
        those values to compute A,B,C

        Alernatively use B value from datasheet which is less accurate but does not require experimetnal data
        https://www.allaboutcircuits.com/projects/measuring-temperature-with-an-ntc-thermistor/
    */ 

    return 1/((1/ROOM_TEMP) + (1/B)*log((thermisterResistance/REFERENCE_RESISTANCE))); // uses B from datasheet
    //return 1/(A + B * log(thermisterResistance) + C * pow(log(thermisterResistance),3)); // uses experimental results
}