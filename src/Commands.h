
namespace commands
{
    bool TryGetGaugeCalibration(int channel, float &calibration);
    bool TryGetGaugeValueRatio(int channel, float &valueRatio);

    void CommandGaugeCalibration();
    void CommandGaugeValueRatio();

    void CommandTriggerChime();
    bool TriggerChime();

    void CommandReset();
    void CommandTogglePrintCANErrors();
    void CommandTogglePrintCANLogs();

    void CommandToggleDisplay();
    bool GetDisplayState();
}