
// Using ESP32-C3 super mini

#define VCCVoltage 3.3f
#define ANALOG_RES 12

// https://github.com/CDFER/esp32-Max-PWM
#define LEDC_RESOLUTION 10
#define LEDC_DUTY_MAX 1024 // 2^LEDC_RESOLUTION
#define LEDC_FREQUENCY 35000

#define CHANNEL_TACH 0
#define CHANNEL_COOL 1
#define CHANNEL_FUEL 2

#define CHANNEL_MAX_TACH 0.955
#define CHANNEL_MAX_COOL 0.8
#define CHANNEL_MAX_FUEL 0.5

// High beams, Turn signals LEDs are controlled from the car. (By using optocouplers)

// Pins:
#define PIN_TACH 0 // 0 A0
#define PIN_COOLANT_LED 1 // 1 A1
#define PIN_COOLANT_GAUGE 2 // 2 A2 STRAPPING must and will be high on boot 10KOhm
#define PIN_AUDIO 3 // 3 A3
// RESERVED 4 A4 SPI_SCK
// RESERVED 5 A5 SPI_SO
// RESERVED 6 SPI_SI

// Multiplexed display and CAN chip select with NOT logic. When display is high(deactivated) the CAN will be low(active) and vice versa
#define PIN_CS_TOGGLE 7 // 7 SPI_SS

#define PIN_FUEL_GAUGE 8 // 8 I2C-SDA STRAPPING BUILT_IN_LED must and will be high on boot 10KOhm
#define PIN_DC_DISPLAY 9 // 9 I2C-SCL STRAPPING will be high on boot 10KOhm
#define PIN_BATTERY_LED 10 // 10
// Need for debug 20 RX
// Need for debug 21 TX