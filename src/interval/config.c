#include <pebble.h>
#include "config.h"
#include "../common/tools.h"

#define ONE_DEFAULT 40 * 60
#define TWO_DEFAULT 50 * 60
#define THREE_DEFAULT 55 * 60
#define FOUR_DEFAULT 60 * 60
#define LENGTH_DEFAULT 150

IntervalSettings interval_settings;

void interval_persist_read(int pkey) {
	int bytesRead = persist_read_data(pkey, &interval_settings, sizeof(IntervalSettings));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "interval:persist:read - pkey: %d, read: %d bytes", pkey, bytesRead);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "interval settings - one: %d, two: %d, three: %d, four: %d", interval_settings.one, interval_settings.two, interval_settings.three, interval_settings.four);
}

void interval_persist_write(int pkey) {
	int bytesWritten = persist_write_data(pkey, &interval_settings, sizeof(IntervalSettings));
	int expectedBytesWritten = sizeof(IntervalSettings);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "interval:persist:write - pkey: %d, expected written: %d bytes. actual written: %d bytes", pkey, expectedBytesWritten, bytesWritten);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "interval settings - one: %d, two: %d, three: %d, four: %d", interval_settings.one, interval_settings.two, interval_settings.three, interval_settings.four);
}

static void interval_persist_reset(int pkey) {
	interval_settings.one = ONE_DEFAULT;
	interval_settings.two = TWO_DEFAULT;
	interval_settings.three = THREE_DEFAULT;
	interval_settings.four = FOUR_DEFAULT;
	interval_settings.length = LENGTH_DEFAULT;

	interval_persist_write(pkey);
}

void interval_persist_migrate(int pkey, int version) {
	switch (version) {
		case 0:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "interval:persist:migrate - unknown version, resetting settings");
			interval_persist_reset(pkey);
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "interval:persist:migrate - already up to date");
			break;
	}
}

char *interval_tostring(char *output, int length) {
	char one_text[7];
	format_time(one_text, interval_settings.one);
	
	char two_text[7];
	format_time(two_text, interval_settings.two);

	char three_text[7];
	format_time(three_text, interval_settings.three);
	
	snprintf(output, length, "%s, %s, %s", one_text, two_text, three_text);
	
	return output;
}

int interval_menu_height() {
	return MENU_CELL_BASIC_HEIGHT;
}

int interval_get_total_time() {
	return interval_settings.four;
}