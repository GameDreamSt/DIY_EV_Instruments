
#include <string>
#include "EVLib/MathUtils.h"

struct EVData
{
    float motorTemperature;
    float inverterTemperature;
    float DCDC_Temperature;
    float OBC_Temperature1;
    float OBC_Temperature2;

    float inverterVoltage;
    float estimatedPowerKW;

    int RPM;

    /*float ambientTemperature;

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

    float chargeTimePassed;
    float chargeTimeRemaining;
    float chargeWattage;

    int RPM;*/

    float GetMaxHeatspotTemperature()
    {
        float temp = motorTemperature;
        temp = Max(temp, inverterTemperature);
        temp = Max(temp, DCDC_Temperature);
        temp = Max(temp, OBC_Temperature1);
        temp = Max(temp, OBC_Temperature2);
        return temp;
    }

    float GetMinHeatspotTemperature()
    {
        float temp = motorTemperature;
        temp = Min(temp, inverterTemperature);
        temp = Min(temp, DCDC_Temperature);
        temp = Min(temp, OBC_Temperature1);
        temp = Min(temp, OBC_Temperature2);
        return temp;
    }

    float GetTemperatureStatus01() // 0 - cold, 0.5 - expected working temp, 1 - "danger to manifold" temperature
    {
        float temp = Remap01(motorTemperature, 0, 80);
        temp = Max(temp, Remap01(inverterTemperature, 0, 70));
        temp = Max(temp, Remap01(DCDC_Temperature, 0, 70));
        temp = Max(temp, Remap01(OBC_Temperature1, 0, 70));
        temp = Max(temp, Remap01(OBC_Temperature2, 0, 70));
        return temp;
    }
};

void TickCAN();
EVData GetEVData();