
#include "CANCommunication.h"

#include "EVLib/CAN.h"
#include "EVLib/CAN/mcp2515.h"
#include "EVLib/MathUtils.h"
#include "EVLib/SerialPrint.h"
#include "EVLib/Timer.h"

#include "Pinout.h"

#include <cstring>

using namespace std;

CAN *can = nullptr;
uint8_t outFrame[8];

EVData evData;
EVData GetEVData()
{
    return evData;
}

enum MsgID
{
    RcvInvTempF = 0x55A,
    RcvInvState = 0x1DA,
    RcvOBC_DCDC_Status = 0x377,
    RcvOBC_Status1 = 0x389,
    RcvOBC_Status2 = 0x38A,
};

bool IsCanIDValid(uint32_t ID)
{
    switch (ID)
    {
    case 0x55A:
    case 0x1DA:
    case 0x377:
    case 0x389:
    case 0x38A:
        return true;
    }

    return false;
}

void ClearCAN()
{
    unsigned long long zero = 0;
    memcpy(&outFrame, &zero, 8);
}

bool PrintCANDataLengthFailure(int ID, int expected, int got)
{
    PrintSerialMessage("Invalid CAN data length for " + IntToHex(ID) + ". Expected: " + ToString(expected) + ", got: ",
                       got);
    return true;
}

static float FahrenheitToCelsius(unsigned char fahrenheitRawValue)
{
    float temperature = (float)fahrenheitRawValue;
    temperature -= 32;
    temperature *= 0.555555f;
    return temperature;
}

can_frame recvFrame;
Timer canTestTimer = Timer(1);
void TickCAN()
{
   if (can == nullptr) // Lazy init so the upload process doesn't hang up
        can = new CAN(PIN_CAN_CS);

    if(canTestTimer.HasTriggered())
    {
        can_frame testFrame;
        testFrame.can_id = 2;
        testFrame.can_dlc = 8;

        for(int i = 0; i < 8; i++)
            testFrame.data[i] = i + 1;

        can->Transmit(testFrame);
    }

    if (!can->GetCanData(recvFrame))
        return;

    uint8_t inFrame[8];
    int inFrameSize = Min(recvFrame.can_dlc, 8);
    memcpy(&inFrame, recvFrame.data, inFrameSize);

    uint32_t canID = recvFrame.can_id;
    if (!IsCanIDValid(canID))
        return;

    MsgID messageType = (MsgID)canID;
    int torque, rpm;
    switch (messageType)
    {
    case MsgID::RcvInvState:
        evData.inverterVoltage = (((uint16_t)inFrame[0] << 2) | (((uint16_t)inFrame[1]) >> 6)) / 2;

        torque = (short)((inFrame[2] & 0x07) << 8 | inFrame[3]);
        if ((inFrame[2] & 0x04) == 0x04) // indicates negative value
            torque = torque | 0xf800;    // pad leading 1s for 2s complement signed
        torque /= 2;

        rpm = (short)(inFrame[4] << 8 | inFrame[5]);
        if ((inFrame[4] & 0x40) == 0x40) // indicates negative value
            rpm = rpm | 0x8000;          // pad leading 1s for 2s complement signed
        rpm /= 2;
        evData.RPM = rpm;

        // https://en.wikipedia.org/wiki/Motor_constants
        evData.estimatedPowerKW = rpm * torque / 13.5047447424f; //  30 * sqrt(2) / pi
        evData.estimatedPowerKW /= 1000.0f;
        break;

    case MsgID::RcvInvTempF:
        evData.motorTemperature = FahrenheitToCelsius(inFrame[4]);
        evData.inverterTemperature = FahrenheitToCelsius(inFrame[2]);
        break;

    case MsgID::RcvOBC_DCDC_Status:
        if(inFrameSize < 8)
           { PrintCANDataLengthFailure(canID, 8, inFrameSize);  return; }
        evData.DCDC_Temperature = inFrame[4] - 40;
        evData.DCDC_Temperature = Max(evData.DCDC_Temperature, inFrame[5] - 40);
        evData.DCDC_Temperature = Max(evData.DCDC_Temperature, inFrame[6] - 40);
        break;

    case MsgID::RcvOBC_Status1:
        if(inFrameSize < 6)
            { PrintCANDataLengthFailure(canID, 6, inFrameSize);  return; }
        evData.OBC_Temperature1 = inFrame[3] - 40;
        evData.OBC_Temperature1 = Max(evData.OBC_Temperature1, inFrame[4] - 40);
        break;

    case MsgID::RcvOBC_Status2:
        if(inFrameSize < 4)
            { PrintCANDataLengthFailure(canID, 4, inFrameSize);  return; }
        evData.OBC_Temperature2 = inFrame[0] - 40;
        evData.OBC_Temperature2 = Max(evData.OBC_Temperature2, inFrame[1] - 40);
        break;

    default:
        break;
    }
}