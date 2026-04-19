
#include "FS.h"
#include <LittleFS.h>

#include "FileUtility.h"
#include "EVLib/SerialPrint.h"
#include <string>

void ListDirectory(const char *dirname, uint8_t levels)
{
    PrintSerialMessage("Listing directory: " + std::string(dirname) + "\r\n");

    File root = LittleFS.open(dirname);
    if (!root)
    {
        PrintSerialMessage("- failed to open directory");
        return;
    }
    if (!root.isDirectory())
    {
        PrintSerialMessage(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file)
    {
        if (file.isDirectory())
        {
            PrintSerialMessage("  DIR : " + std::string(file.name()));
            if (levels)
                ListDirectory(file.path(), levels - 1);
        }
        else
        {
            PrintSerialMessage("  FILE: " + std::string(file.name()) + "\tSIZE: " + ToString(file.size()));
        }
        file = root.openNextFile();
    }
}