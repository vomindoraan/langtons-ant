// Teensy++ 2.0
#include <SPI.h>
#include <TFT.h>

#define COLOR_COUNT          14
#define BYTES_PER_PIXEL      3
#define COLOR_RULE_FMT       "{%02hhx%02hhx%02hhx,%c}"  // {RRGGBB,T}
#define COLOR_RULE_LEN       (BYTES_PER_PIXEL*2 + 1 + 3)
#define COLOR_RULES_MSG_LEN  (COLOR_COUNT * COLOR_RULE_LEN)
#define COLOR_RULES_MSG_SIZE (COLOR_RULES_MSG_LEN + 1)

#define WHITE             0xFFFFFF
#define GRAY              0x808080
#define IS_WHITE(r, g, b) ((r) == 0xFF && (g) == 0xFF && (b) == 0xFF)

#define CS0  PIN_C7
#define CS1  PIN_C6
#define CS2  PIN_C5
#define CS3  PIN_C4
#define CS4  PIN_C3
#define CS5  PIN_C2
#define CS6  PIN_C1
#define CS7  PIN_C0
#define CS8  PIN_E1
#define CS9  PIN_D4
#define CS10 PIN_D5
#define CS11 PIN_D6
#define CS12 PIN_D7
#define CS13 PIN_E0
#define DC   PIN_E7
#define RST  -1  // Unused

TFT displays[] = {
    TFT(CS0,  DC, RST),
    TFT(CS1,  DC, RST),
    TFT(CS2,  DC, RST),
    TFT(CS3,  DC, RST),
    TFT(CS4,  DC, RST),
    TFT(CS5,  DC, RST),
    TFT(CS6,  DC, RST),
    TFT(CS7,  DC, RST),
    TFT(CS8,  DC, RST),
    TFT(CS9,  DC, RST),
    TFT(CS10, DC, RST),
    TFT(CS11, DC, RST),
    TFT(CS12, DC, RST),
    TFT(CS13, DC, RST),
};

void setup() {
    Serial.begin(115200);
    for (int i = 0; i < COLOR_COUNT; i++) {
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
        tft.setRotation((index >= COLOR_COUNT/2) ? 1 : 3);
        unsigned turnColor = IS_WHITE(r, g, b) ? GRAY : WHITE;
        tft.drawChar(52, 30, turn, turnColor, turnColor, 10);
        index++;
    }

    for (; index < COLOR_COUNT; index++) {
        auto& tft = displays[index];
        tft.background(0, 0, 0);
    }
}
