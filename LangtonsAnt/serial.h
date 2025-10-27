/**
 * @file serial.h
 * Extension for data input/output via serial
 * Requires external script [-DSERIAL_SCRIPT] and -DSERIAL_COLORS/-D...
 * @author vomindoraan
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "io.h"

/** Compile flags (serial disabled by default) */
///@{
#if defined(SERIAL_COLORS_ON)
#	define SERIAL_COLORS  1
#elif !defined(SERIAL_COLORS)
#	define SERIAL_COLORS  0
#endif

#ifndef SERIAL_SCRIPT
#	define SERIAL_SCRIPT  "scripts/write_serial.py"
#endif
///@}

#if SERIAL_COLORS

#include "io.h"


/*---------------------- Serialization macros and types ----------------------*/

/** Serialized color rules, format: {RRGGBB,T} */
///@{
#define COLOR_RULE_FMT       "{%02hhx%02hhx%02hhx,%c}"
#define COLOR_RULE_LEN       (BYTES_PER_PIXEL*2 + 1 + 3)
#define COLOR_RULE_SZ        (COLOR_RULE_LEN + 1)

#define COLOR_RULES_COUNT    (COLOR_COUNT - 2)  // def_color, COLOR_NONE
#define COLOR_RULES_MSG_LEN  (COLOR_RULES_COUNT * COLOR_RULE_LEN)
#define COLOR_RULES_MSG_SZ   (COLOR_RULES_MSG_LEN + 1)
///@}

/** Single (color, turn) pair */
typedef struct color_rule {
	color_t  color;
	turn_t   turn;
} ColorRule;

/** Serialization types */
///@{
typedef ColorRule  ColorRules[COLOR_RULES_COUNT];
typedef char       ColorRulesMsg[COLOR_RULES_MSG_SZ];
///@}


/*----------------------------------------------------------------------------*
 *                                  serial.c                                  *
 *----------------------------------------------------------------------------*/

bool colors_to_color_rules(Colors *colors, ColorRules rules);
bool is_color_rule_valid(ColorRule rule);
void serialize_color_rules(ColorRules rules, ColorRulesMsg msg);
//bool deserialize_color_rules(ColorRulesMsg msg, ColorRules rules);  // TODO
bool serial_send_colors(Colors *colors);

#endif  // SERIAL_COLORS

#endif  // __SERIAL_H__
