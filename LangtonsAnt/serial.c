#include "serial.h"

#ifdef SERIAL_COLORS

#include <stdio.h>
#include <string.h>

bool colors_to_color_rules(Colors *colors, ColorRules rules)
{
	color_t c;
	bool do_for = TRUE;
	int i = 0;

	if (!colors) {
		return FALSE;
	}

	/* Add existing colors to rules */
	for (c = colors->first; do_for; c = colors->next[c]) {
		if (c == COLOR_NONE) {
			break;
		}
		rules[i++] = (ColorRule) { c, colors->turn[c] };
		do_for = c != colors->last;
	}

	/* Mark remaining color rules as invalid */
	for (; i < COLOR_COUNT; i++) {
		rules[i] = (ColorRule) { COLOR_NONE, TURN_NONE };
	}

	return TRUE;
}

bool is_color_rule_valid(ColorRule rule)
{
	return rule.color != COLOR_NONE && rule.color != TURN_NONE;
}

void serialize_color_rules(ColorRules rules, ColorRulesMsg msg)
{
	int i, j = 0;
	msg[0] = '\0';
	for (i = 0; i < COLOR_COUNT && is_color_rule_valid(rules[i]); i++) {
		color_t c = rules[i].color;
		turn_t t = rules[i].turn;
		const pixel_t *pbgr = &color_map[c];
		char tmp[COLOR_RULE_LEN+1];
		snprintf(tmp, COLOR_RULE_LEN+1, COLOR_RULE_FMT,
		         (*pbgr)[0], (*pbgr)[1], (*pbgr)[2], turn2arrow(t) & 0xFF);
		strcat(msg, tmp);
	}
}

// TODO
//bool deserialize_color_rules(ColorRulesMsg str, ColorRules rules)
//{
//	int i = 0;
//	pixel_t pbgr;
//	turn_t turn;
//	while (sscanf(str, COLOR_RULE_FMT, &pbgr[0], &pbgr[1], &pbgr[2], &turn) == 4) {
//	}
//}

bool serial_send_colors(Colors *colors)
{
	FILE *pipe;
	ColorRules rules;
	ColorRulesMsg msg;

	colors_to_color_rules(colors, rules);
	serialize_color_rules(rules, msg);

#ifdef _WIN32
	pipe = _popen(SERIAL_SCRIPT, "w");
#else
	pipe = popen(SERIAL_SCRIPT, "w");
#endif
	if (!pipe) {
		return FALSE;
	}

	bool success = fputs(msg, pipe) != EOF;
#ifdef _WIN32
	_pclose(pipe);
#else
	pclose(pipe);
#endif
	return success;
}

#endif // SERIAL_COLORS
