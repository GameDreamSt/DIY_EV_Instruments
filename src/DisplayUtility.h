
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
    string text;
    int ox, oy, ow, oh;

  public:
    Rect rect;
    uint8_t textSize;

    Label() : text(""), rect(0, 0, 0, 0, Anchor::MiddleCenter)
    {
        ox = oy = ow = oh = 0;
        textSize = 1;
    }

    void Draw()
    {
        tft.fillRect(ox, oy, ow, oh, Display_Backround_Color);

        int16_t x, y;
        uint16_t w, h;
        tft.setTextSize(textSize);
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

    void SetText(const char newText[])
    {
        SetText(string(newText));
    }
    void SetText(string newText)
    {
        if (newText == text)
            return;
        text = newText;
        Draw();
    }
};

struct Sprite
{
private:
    string path;
public:
    Rect rect;

    Sprite(string path) : rect(0, 0, 0, 0, Anchor::MiddleCenter), path(path) {}

    bool Draw()
    {
        File fp = LittleFS.open(path.c_str(), "r");

        if (!fp)
        {
            PrintSerialMessage("Can't display sprite, '" + path + "' is missing!");
            return false;
        }

        size_t availableBytes = fp.available();

        if (availableBytes == 0)
        {
            PrintSerialMessage("Can't display sprite, 0 bytes!");
            fp.close();
            return false;
        }

        size_t bytesLeftInRam = ESP.getFreeHeap();
        if (availableBytes >= bytesLeftInRam)
        {
            PrintSerialMessage("Can't display sprite, not enough memory! Left:" + ToString(bytesLeftInRam) +
                            ", needed: " + ToString(availableBytes));
            fp.close();
            return false;
        }

        uint32_t width, height;
        fp.read((uint8_t *)&width, sizeof(uint32_t));
        fp.read((uint8_t *)&height, sizeof(uint32_t));

        availableBytes = fp.available();
        uint16_t *RGB565Data = new uint16_t[availableBytes / sizeof(uint16_t)];
        fp.read((uint8_t *)RGB565Data, availableBytes);
        fp.close();

        PrintSerialMessage("Sprite '" + path + "' is: " + ToString((size_t)width) + "x" + ToString((size_t)height));

        rect.elementWidth = width;
        rect.elementHeight = height;

        int x, y;
        rect.GetTransformedValues(x, y);
        tft.drawRGBBitmap(x, y, RGB565Data, width, height);

        delete RGB565Data;
        return true;
    }
};