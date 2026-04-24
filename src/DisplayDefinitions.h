
Label coolantTemperatureLabel;
Label timeLabel;
Label ambientTemperatureLabel;

Label rangeBestLabel;
Label rangeAverageLabel;
Label rangeWorstLabel;

Label chargeTimeLeftLabel;
Label chargeTimePassedLabel;
Label chargeWattageLabel;

Label consumption100KmLabel;
Label tripTextLabel, consumptionTripLabel;
Label totalTextLabel, consumptionTotalLabel;

Label auxBatteryAmperageLabel;
Label auxBatteryVoltageLabel;

Label tractionBatteryTemperatureLabel;
Label tractionBatteryWattageLabel;
Label tractionBatteryVoltageLabel;
Label tractionBatterySOCLabel;
Label tractionBatteryCapacityLabel;

const int headerHeight = 40;
const int rangeSpacing = 35;
const int size1Height = 17;
const int size2Height = 17 * 2;
const int size1Spacing = 5;

bool initialLinesDrawn;
struct timeval displayTimeStruct;

void InitializeDisplayDefinitions()
{
    int leftEdgePadding = 10;
    int rightEdgePadding = 14 + leftEdgePadding;
    coolantTemperatureLabel.rect = Rect(leftEdgePadding, 0, displayWidth, headerHeight, Anchor::UpperLeft);
    timeLabel.rect = Rect(0, 0, displayWidth, headerHeight, Anchor::UpperCenter);
    timeLabel.textSize = 2;
    ambientTemperatureLabel.rect = Rect(0, 0, displayWidth - rightEdgePadding, displayHeight, Anchor::UpperRight);

    rangeBestLabel.rect = Rect(0, headerHeight, displayWidth, rangeSpacing, Anchor::MiddleCenter);
    rangeAverageLabel.rect = Rect(0, headerHeight + rangeSpacing, displayWidth, rangeSpacing, Anchor::MiddleCenter);
    rangeAverageLabel.textSize = 2;
    rangeWorstLabel.rect = Rect(0, headerHeight + 2 * rangeSpacing, displayWidth, rangeSpacing, Anchor::MiddleCenter);

    chargeTimeLeftLabel.rect = Rect(0, headerHeight + size2Height, displayWidth, size2Height, Anchor::MiddleCenter);
    chargeTimePassedLabel.rect = Rect(0, headerHeight + 2 * size2Height, displayWidth, size1Height, Anchor::MiddleCenter);
    chargeWattageLabel.rect = Rect(0, headerHeight + 2 * size2Height + size1Height, displayWidth, size1Height, Anchor::MiddleCenter);

    int width = displayWidth - 64;
    
    int height0 = displayHeight - size1Spacing - 20;
    int height1 = height0 - size1Spacing - size1Height;
    int height2 = height1 - size1Spacing - size1Height;
    int height3 = height2 - size1Spacing - size1Height;
    int height4 = height3 - size1Spacing - size1Height;

    auxBatteryAmperageLabel.rect = Rect(leftEdgePadding, 0, displayWidth, height4, Anchor::LowerLeft);
    auxBatteryVoltageLabel.rect = Rect(leftEdgePadding, 0, displayWidth, height3, Anchor::LowerLeft);
    consumption100KmLabel.rect = Rect(leftEdgePadding, 0, width, height2, Anchor::LowerLeft);
    consumptionTripLabel.rect = Rect(80, 0, width, height1, Anchor::LowerLeft);
    consumptionTotalLabel.rect = Rect(80, 0, width, height0, Anchor::LowerLeft);

    tripTextLabel.rect = Rect(leftEdgePadding, 0, displayWidth, height1, Anchor::LowerLeft);
    totalTextLabel.rect = Rect(leftEdgePadding, 0, displayWidth, height0, Anchor::LowerLeft);

    tractionBatteryTemperatureLabel.rect = Rect(0, 0, displayWidth - rightEdgePadding, height4, Anchor::LowerRight);
    tractionBatteryVoltageLabel.rect = Rect(0, 0, displayWidth - rightEdgePadding, height3, Anchor::LowerRight);
    tractionBatteryWattageLabel.rect = Rect(0, 0, displayWidth - rightEdgePadding, height2, Anchor::LowerRight);
    tractionBatterySOCLabel.rect = Rect(0, 0, displayWidth - rightEdgePadding, height1, Anchor::LowerRight);
    tractionBatteryCapacityLabel.rect = Rect(0, 0, displayWidth - rightEdgePadding, height0, Anchor::LowerRight);
}

string TimeToString(int value)
{
    if(value < 10)
        return "0" + ToString(value);

    return ToString(value);
}

void SetDefinitionsText()
{
    gettimeofday(&displayTimeStruct, NULL);
    uint64_t secondsTotal = displayTimeStruct.tv_sec;
    int seconds = secondsTotal % 60;
    int minutes = (secondsTotal / 60) % 60;
    int hours = (secondsTotal / 3600) % 24;

    coolantTemperatureLabel.SetText("20 C");
    timeLabel.SetText(TimeToString(hours) + ":" + TimeToString(minutes));
    ambientTemperatureLabel.SetText("8 C");

    rangeBestLabel.SetText("130 km");
    rangeAverageLabel.SetText("100 km");
    rangeWorstLabel.SetText("60 km");

    /*chargeTimeLeftLabel.SetText("");
    chargeTimePassedLabel.SetText("");
    chargeWattageLabel.SetText("");*/

    consumption100KmLabel.SetText("100 km/Wh");
    tripTextLabel.SetText("Trip");
    consumptionTripLabel.SetText("180");
    totalTextLabel.SetText("Total");
    consumptionTotalLabel.SetText("130");

    auxBatteryAmperageLabel.SetText("6.3 A");
    auxBatteryVoltageLabel.SetText("14.4 V");

    tractionBatteryTemperatureLabel.SetText("18 C");
    tractionBatteryWattageLabel.SetText("22.1 kW");
    tractionBatteryVoltageLabel.SetText("232 V");
    tractionBatterySOCLabel.SetText("23.6% SOC");
    tractionBatteryCapacityLabel.SetText("16.2/24.0 kWh");

    if(!initialLinesDrawn)
    {
        initialLinesDrawn = true;
        tft.writeFastHLine(0, headerHeight - 5, displayWidth, Display_Color_White);
    }
}