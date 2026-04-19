
#include <string>

struct EVData
{
    float ambientTemperature;
    float coolantTemperature;

    float fuelRemainingRatio;

    float tractionBatteryVoltage;
    float tractionBatteryAmperage;
    float tractionBatteryTemperature;
    float tractionBatterySOC;

    float auxBatteryVoltage;
    float auxBatteryAmperage;

    float rangeEstimateWorst;
    float rangeEstimateAverage;
    float rangeEstimateBest;

    float tripWhPerKm;
    float totalWhPerKm;

    float chargeTimeLeft;
    float chargeTimeRemaining;
    float chargeWattage;

    int RPM;
};

void TickCAN();
EVData GetEVData();