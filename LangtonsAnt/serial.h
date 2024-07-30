/**
 * @file serial.h
 * Extension for data input/output via serial
 * Requires external script [-DSERIAL_SCRIPT] and -DSERIAL_COLORS/-D...
 * @author vomindoraan
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "io.h"

#ifndef SERIAL_SCRIPT
#	define SERIAL_SCRIPT "./write_serial.py"
#endif

#ifdef SERIAL_COLORS

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

bool serial_send_colors(Colors *colors);

#endif // SERIAL_COLORS

#endif // __SERIAL_H__
