
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

void InitializeGauges();
void TickGauges();