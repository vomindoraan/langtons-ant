#include "serial.h"
#include "io.h"

#include <stdio.h>
#include <string.h>

bool colors_to_color_rules(Colors *colors, ColorRules rules)
{
	color_t c;
	int i = 0;

	if (!colors) {
		return FALSE;
	}

	/* Add existing colors to rules */
	c = colors->first;
	do {
		if (c == COLOR_NONE) {
			break;
		}
		rules[i++] = (ColorRule) { c, colors->turn[c] };
		c = colors->next[c];
	} while (c != colors->last);

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
		char tmp[COLOR_RULE_SIZE+1];
		snprintf(tmp, COLOR_RULE_SIZE+1, COLOR_RULES_MSG_FMT,
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
//	while (sscanf(str, COLOR_RULES_MSG_FMT, &pbgr[0], &pbgr[1], &pbgr[2], &turn) == 4) {
//	}
//}

#ifdef __linux__
bool serial_send_colors(Colors *colors)
{
	FILE *pipe;
	ColorRules rules;
	ColorRulesMsg msg;

	colors_to_color_rules(colors, &rules);
	serialize_color_rules(rules, msg);

	if ((pipe = popen(SERIAL_SCRIPT, "w")) == EOF) {
		return FALSE;
	}

	bool success = fputs(msg, pipe) != EOF;
	pclose(pipe);
	return success;
}
#endif
