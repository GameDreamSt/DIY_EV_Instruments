
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
    // Inverter IDs
    CmdHeartBeat = 0x50B,
};

bool IsCanIDValid(uint32_t ID)
{
    switch (ID)
    {
    case 0x50B:
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

    byte inFrame[8];
    memcpy(&inFrame, recvFrame.data, Min(recvFrame.can_dlc, 8));

    uint32_t canID = recvFrame.can_id;
    if (!IsCanIDValid(canID))
        return;

    MsgID messageType = (MsgID)canID;

    // Handle CAN message
    switch (messageType)
    {
    case MsgID::CmdHeartBeat:
        //PrintSerialMessage("CAN received " + BytesToString(&inFrame, 8));
        break;

    default:
        break;
    }
}