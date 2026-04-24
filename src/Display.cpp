
#include "Display.h"
#include "Pinout.h"

#include "EVLib/SerialPrint.h"
#include "EVLib/Timer.h"

#include "Display/Adafruit_GFX.h"
#include "Display/Adafruit_ST7789.h"
#include "Display/Fonts/FreeSans12pt7b.h"
#include <SPI.h>

#include "FS.h"
#include "esp_system.h"
#include <LittleFS.h>

#include "time.h"
#include "esp_sntp.h"

#include <string>

using namespace std;

int displayWidth;
int displayHeight;
bool initialized;

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, PIN_DISPLAY_CS, PIN_DISPLAY_DC, -1);

// color definitions
const uint16_t Display_Color_Black = ST77XX_BLACK;
const uint16_t Display_Color_Blue = ST77XX_BLUE;
const uint16_t Display_Color_Red = ST77XX_RED;
const uint16_t Display_Color_Green = ST77XX_GREEN;
const uint16_t Display_Color_Cyan = ST77XX_CYAN;
const uint16_t Display_Color_Magenta = ST77XX_MAGENTA;
const uint16_t Display_Color_Yellow = ST77XX_YELLOW;
const uint16_t Display_Color_White = ST77XX_WHITE;
const uint16_t Display_Color_Orange = ST77XX_ORANGE;

// The colors we actually want to use
uint16_t Display_Text_Color = ST77XX_WHITE;
uint16_t Display_Backround_Color = ST77XX_BLACK;

enum State
{
    DisplaySetup,
    Logo,
    Active,
};

#include "DisplayUtility.h"
#include "DisplayDefinitions.h"

State currentState = State::DisplaySetup;
Timer logoTimer = Timer(3);
Timer timer = Timer(1);

bool TryDisplayLogoGraphics(int &logoHeightOffset)
{
    logoHeightOffset = displayHeight / 2;

    File fp = LittleFS.open("/logo.RGB565", "r");

    if (!fp)
    {
        PrintSerialMessage("Can't display logo, missing logo.RGB565!");
        return false;
    }

    size_t availableBytes = fp.available();

    if (availableBytes == 0)
    {
        PrintSerialMessage("Can't display logo, 0 bytes!");
        fp.close();
        return false;
    }

    size_t bytesLeftInRam = ESP.getFreeHeap();
    if (availableBytes >= bytesLeftInRam)
    {
        PrintSerialMessage("Can't display logo, not enough memory! Left:" + ToString(bytesLeftInRam) +
                           ", needed: " + ToString(availableBytes));
        fp.close();
        return false;
    }

    PrintSerialMessage("Bytes left in RAM:" + ToString(bytesLeftInRam) +
                       ", needed for logo: " + ToString(availableBytes));

    uint32_t width, height;
    fp.read((uint8_t *)&width, sizeof(uint32_t));
    fp.read((uint8_t *)&height, sizeof(uint32_t));

    availableBytes = fp.available();
    uint16_t *RGB565Data = new uint16_t[availableBytes / sizeof(uint16_t)];
    fp.read((uint8_t *)RGB565Data, availableBytes);
    fp.close();

    PrintSerialMessage("Logo is: " + ToString((size_t)width) + "x" + ToString((size_t)height));

    int rectHeight = height + 4;
    logoHeightOffset = rectHeight;
    Rect logoRect = Rect(0, 0, displayWidth, rectHeight, width, height, Anchor::MiddleCenter);

    int x, y;
    logoRect.GetTransformedValues(x, y);
    tft.drawRGBBitmap(x, y, RGB565Data, width, height);

    delete RGB565Data;
    return true;
}

void DisplayLogo()
{
    int logoHeightOffset;
    TryDisplayLogoGraphics(logoHeightOffset);

    Label title;
    title.SetText("VOLKSWAGEN");
    title.rect = Rect(0, logoHeightOffset, displayWidth, displayHeight - logoHeightOffset, Anchor::MiddleCenter);
    title.Draw();
}

void InitializeDisplay()
{
    PrintSerialMessage("Initializing display");

    // using a 2.0" 320x240 TFT:
    tft.init(240, 320); // Init ST7789 320x240

    // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
    // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
    // may end up with a black screen some times, or all the time.
    tft.setSPISpeed(10000000);

    tft.setFont(&FreeSans12pt7b);
    tft.fillScreen(Display_Backround_Color);
    tft.setTextColor(Display_Text_Color);
    tft.setTextSize(1);
    tft.setTextWrap(false);
    tft.setRotation(3);
    tft.enableDisplay(false);

    displayWidth = 340; // due to rotation
    displayHeight = 240;

    PrintSerialMessage("Display initialized");

    InitializeDisplayDefinitions();
}

void TickDisplay()
{
    if(!initialized)
    {
        initialized = true;
        InitializeDisplay();
        return;
    }

    switch (currentState)
    {
    case State::DisplaySetup:
        currentState = State::Logo;
        DisplayLogo();
        tft.enableDisplay(true);
        break;

    case State::Logo:
        if (!logoTimer.HasTriggered())
            return;
        currentState = State::Active;
        tft.fillScreen(Display_Backround_Color);
        break;

    case State::Active:
        if (!timer.HasTriggered())
            return;
        SetDefinitionsText();
        break;
    }
}