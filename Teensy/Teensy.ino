// Teensy++ 2.0
#include <TFT.h>

extern "C" {
#define  SERIAL_COLORS  1
#include "serial.h"
}

#define DISPLAY_COUNT      COLOR_RULES_COUNT
#define WHITE              0xFFFFFF
#define GRAY               0x808080
#define IS_WHITE(r, g, b)  ((r) == 0xFF && (g) == 0xFF && (b) == 0xFF)

#define _CS0   PIN_C7
#define _CS1   PIN_C6
#define _CS2   PIN_C5
#define _CS3   PIN_C4
#define _CS4   PIN_C3
#define _CS5   PIN_C2
#define _CS6   PIN_C1
#define _CS7   PIN_C0
#define _CS8   PIN_E1
#define _CS9   PIN_D4
#define _CS10  PIN_D5
#define _CS11  PIN_D6
#define _CS12  PIN_D7
#define _CS13  PIN_E0
#define _DC    PIN_E7
#define _RST   -1  // Unused

TFT displays[] = {
    TFT(_CS0,  _DC, _RST),
    TFT(_CS1,  _DC, _RST),
    TFT(_CS2,  _DC, _RST),
    TFT(_CS3,  _DC, _RST),
    TFT(_CS4,  _DC, _RST),
    TFT(_CS5,  _DC, _RST),
    TFT(_CS6,  _DC, _RST),
    TFT(_CS7,  _DC, _RST),
    TFT(_CS8,  _DC, _RST),
    TFT(_CS9,  _DC, _RST),
    TFT(_CS10, _DC, _RST),
    TFT(_CS11, _DC, _RST),
    TFT(_CS12, _DC, _RST),
    TFT(_CS13, _DC, _RST),
};

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < DISPLAY_COUNT; i++) {
        auto& tft = displays[i];
        tft.begin();
        tft.background(0, 0, 0);
    }
}

void loop() {
    while (!Serial.available());

    String msg = Serial.readString(COLOR_RULES_MSG_LEN);
    const char *begin = msg.c_str();
    int index = 0;

    for (const char *p = begin; p < begin + msg.length(); p += COLOR_RULE_LEN) {
        byte r, g, b;
        char turn;
        if (sscanf(p, COLOR_RULE_FMT, &r, &g, &b, &turn) != 4) {
            break;
        }

        auto& tft = displays[index];
        tft.background(r, g, b);
        tft.setRotation((index >= DISPLAY_COUNT/2) ? 1 : 3);
        unsigned turnColor = IS_WHITE(r, g, b) ? GRAY : WHITE;
        tft.drawChar(52, 30, turn, turnColor, turnColor, 10);
        index++;
    }

    for (; index < DISPLAY_COUNT; index++) {
        auto& tft = displays[index];
        tft.background(0, 0, 0);
    }
}
