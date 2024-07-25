#ifndef __SERIAL__
#define __SERIAL__

#include "io.h"

/* Message format: {BBGGRR,T} */
#define COLOR_RULE_FMT       "{%02hhx%02hhx%02hhx,%c}"
#define COLOR_RULE_LEN       (BYTES_PER_PIXEL*2 + 1 + 3)
#define COLOR_RULES_MSG_LEN  (COLOR_COUNT * COLOR_RULE_LEN)
#define COLOR_RULES_MSG_SIZE (COLOR_RULES_MSG_LEN + 1)

typedef struct color_rule {
	color_t color;
	turn_t turn;
} ColorRule;

typedef ColorRule ColorRules[COLOR_COUNT];
typedef byte ColorRulesMsg[COLOR_RULES_MSG_SIZE];

bool colors_to_color_rules(Colors *colors, ColorRules rules);

bool is_color_rule_valid(ColorRule rule);

void serialize_color_rules(ColorRules rules, ColorRulesMsg msg);

// TODO
//bool deserialize_color_rules(ColorRulesMsg msg, ColorRules rules);

#ifdef __linux__
#	define SERIAL_SCRIPT "./write_serial.py"

bool serial_send_colors(Colors *colors);

#endif

#endif
