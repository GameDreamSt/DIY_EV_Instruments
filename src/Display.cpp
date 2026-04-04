
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

#include <string>

using namespace std;

int displayWidth;
int displayHeight;

Adafruit_ST7789 tft = Adafruit_ST7789(&SPI, PIN_CS_TOGGLE, PIN_DC_DISPLAY, -1);

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

enum Anchor
{
    UpperLeft = 0,
    UpperCenter = 1,
    UpperRight = 2,
    MiddleLeft = 3,
    MiddleCenter = 4,
    MiddleRight = 5,
    LowerLeft = 6,
    LowerCenter = 7,
    LowerRight = 8,
};

struct Rect
{
    int posX, posY;
    int rectWidth, rectHeight;
    int elementWidth, elementHeight;
    Anchor alignment;

    Rect(int positionX, int positionY, int rectWidth, int rectHeight, Anchor anchorAlignment)
        : posX(positionX), posY(positionY), rectWidth(rectWidth), rectHeight(rectHeight), alignment(anchorAlignment)
    {
        elementWidth = 0;
        elementHeight = 0;
    }

    Rect(int positionX, int positionY, int rectWidth, int rectHeight, int elementWidth, int elementHeight,
         Anchor anchorAlignment)
        : posX(positionX), posY(positionY), rectWidth(rectWidth), rectHeight(rectHeight), elementWidth(elementWidth),
          elementHeight(elementHeight), alignment(anchorAlignment)
    {
    }

    void GetTransformedValues(int &positionX, int &positionY)
    {
        int pivotX;
        int pivotY;
        switch (alignment)
        {
        case Anchor::UpperLeft:
        case Anchor::MiddleLeft:
        case Anchor::LowerLeft:
            positionX = pivotX = posX;
            break;

        case Anchor::UpperCenter:
        case Anchor::MiddleCenter:
        case Anchor::LowerCenter:
            pivotX = posX + rectWidth / 2;
            positionX = pivotX - elementWidth / 2;
            break;

        case Anchor::UpperRight:
        case Anchor::MiddleRight:
        case Anchor::LowerRight:
            pivotX = posX + rectWidth;
            positionX = pivotX - elementWidth;
            break;
        }

        switch (alignment)
        {
        case Anchor::UpperLeft:
        case Anchor::UpperCenter:
        case Anchor::UpperRight:
            positionY = pivotY = posY;
            break;

        case Anchor::MiddleLeft:
        case Anchor::MiddleCenter:
        case Anchor::MiddleRight:
            pivotY = posY + rectHeight / 2;
            positionY = pivotY - elementHeight / 2;
            break;

        case Anchor::LowerLeft:
        case Anchor::LowerCenter:
        case Anchor::LowerRight:
            pivotY = posY + rectHeight;
            positionY = pivotY - elementHeight / 2;
            break;
        }
    }
};

struct Label
{
  private:
    bool dirty;
    string text;
    int ox, oy, ow, oh;

  public:
    Rect rect;

    Label() : text(""), rect(0, 0, 0, 0, Anchor::MiddleCenter)
    {
        dirty = true;
        ox = oy = ow = oh = 0;
    }

    void SetText(const char newText[])
    {
        SetText(string(newText));
    }
    void SetText(string newText)
    {
        if (newText == text)
            return;
        text = newText;
        dirty = true;
    }

    void Draw(bool force = false)
    {
        if (!force && !dirty)
            return;

        tft.fillRect(ox, oy, ow, oh, Display_Backround_Color);

        int16_t x, y;
        uint16_t w, h;
        tft.getTextBounds(text.c_str(), 0, 0, &x, &y, &w, &h);

        rect.elementWidth = w;
        rect.elementHeight = h;

        int tx, ty;
        rect.GetTransformedValues(tx, ty);
        ox = tx;
        oy = ty;
        ow = w;
        oh = h;
        /*PrintSerialMessage("X:" + ToString(x) +
        " Y:" + ToString(y) +
        " TX:" + ToString(tx) +
        " TY:" + ToString(ty) +
        " W:" + ToString(w) +
        " H:" + ToString(h));*/
        tx -= x;
        ty -= y;

        tft.setCursor(tx, ty);
        tft.setTextColor(Display_Text_Color);
        tft.print(text.c_str());
    }
};

State currentState = State::DisplaySetup;
Timer logoTimer = Timer(3);
Timer timer = Timer(1);
Label timeLabel;

void displayUpTime()
{
    unsigned long upSeconds = millis() / 1000;
    unsigned long days = upSeconds / 86400;
    upSeconds = upSeconds % 86400;
    unsigned long hours = upSeconds / 3600;
    upSeconds = upSeconds % 3600;
    unsigned long minutes = upSeconds / 60;
    upSeconds = upSeconds % 60;

    char newTimeString[16] = {0};
    sprintf(newTimeString, "%lu %02lu:%02lu:%02lu", days, hours, minutes, upSeconds);

    timeLabel.SetText(string(newTimeString));
    timeLabel.Draw();
}

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
    // tft.setSPISpeed(40000000);

    tft.setFont(&FreeSans12pt7b);
    tft.fillScreen(Display_Backround_Color);
    tft.setTextColor(Display_Text_Color);
    tft.setTextSize(1);
    tft.setRotation(3);
    tft.enableDisplay(false);

    displayWidth = 340; // due to rotation
    displayHeight = 240;

    PrintSerialMessage("Display initialized");

    timeLabel = Label();
    timeLabel.rect = Rect(0, 0, displayWidth, displayHeight, Anchor::UpperLeft);
}

void TickDisplay()
{
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

        displayUpTime();
        break;
    }
}