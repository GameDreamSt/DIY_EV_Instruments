
class Gauge
{
  private:
  int channel;
  float oldValue;
  float calibration;

  public:
    Gauge();
    Gauge(int setPin, int setChannel);
    void SetValueRatio(float ratio);
    void SetCalibration(float maxRatio);
};

/*
void TickTach()
{
    int desiredRPM = commands::GetDesiredRPM();
    if(oldRPM != desiredRPM)
    {
        oldRPM = desiredRPM;
        desiredRPM = constrain(desiredRPM, 0, MAX_RPM);
        float dutyFloat = desiredRPM / (float)MAX_RPM;
        int duty = LEDC_DUTY_MAX * dutyFloat;
        ledcWriteChannel(CHANNEL_TACH, duty);
        PrintSerialMessage("Duty chosen for RPM: " + FloatToString(dutyFloat * 100, 2) + "");
    }
}

void DoTachSweep()
{
    int seconds = 5;
    int microsecondsToDelay = seconds * 1000000 / LEDC_DUTY_MAX;
    for (int i = 0; i < LEDC_DUTY_MAX; i++)
    {
        ledcWriteChannel(CHANNEL_TACH, i);
        delayMicroseconds(microsecondsToDelay);
    }

    for (int i = LEDC_DUTY_MAX - 1; i >= 0; i--)
    {
        ledcWriteChannel(CHANNEL_TACH, i);
        delayMicroseconds(microsecondsToDelay);
    }

    PrintSerialMessage("Wrote full " + ToString(LEDC_DUTY_MAX) + " values, repeating...");
}
*/