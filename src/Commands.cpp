
#include "Commands.h"
#include "EVLib/SerialReader.h"
#include "EVLib/SerialPrint.h"

using namespace std;

namespace commands
{
    struct GaugeValue
    {
        int channel;
        float value;

        GaugeValue(int channel, float value) : channel(channel), value(value) {}
    };

    vector<GaugeValue> calibrationValues;
    vector<GaugeValue> overrideValues;
    bool triggerChime;

    bool TryGetGaugeValue(vector<GaugeValue> &gaugeValues, int channel, float &value)
    {
        for(int i = 0; i < gaugeValues.size(); i++)
        {
            if(gaugeValues[i].channel != channel)
                continue;

            value = gaugeValues[i].value;
            return true;
        }
        return false;
    }

    bool TryGetGaugeCalibration(int channel, float &calibration)
    {
        return TryGetGaugeValue(calibrationValues, channel, calibration);
    }
    bool TryGetGaugeValueRatio(int channel, float &valueRatio)
    {
        return TryGetGaugeValue(overrideValues, channel, valueRatio);
    }

    void ProcessGaugeCommand(vector<GaugeValue> &gaugeValues)
    {
        vector<string> parameters = *GetParameters();
        if(parameters.size() <= 1)
        {
            PrintSerialMessage("Not enough parameters!");
            return;
        }

        int channel = std::stoi(parameters[0]);
        float value = std::stof(parameters[1]);

        for(int i = 0; i < gaugeValues.size(); i++)
        {
            if(gaugeValues[i].channel != channel)
                continue;

            gaugeValues[i].value = value;
            return;
        }

        gaugeValues.push_back(GaugeValue(channel, value));
    }

    void CommandGaugeCalibration()
    {
        ProcessGaugeCommand(calibrationValues);
    }
    void CommandGaugeValueRatio()
    {
        ProcessGaugeCommand(overrideValues);
    }

    void CommandTriggerChime()
    {
        triggerChime = true;
    }

    bool TriggerChime()
    {
        bool wasTrue = triggerChime;
        triggerChime = false;
        return wasTrue;
    }

    void(* Reset) (void) = 0;
    void CommandReset()
    {
        Reset();
    }
}