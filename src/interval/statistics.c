#include <pebble.h>
#include "statistics.h"
#include "../common/tools.h"

Statistics statistics;

void statistics_persist_read(int pkey) {
    int bytesRead = persist_read_data(pkey, &statistics, sizeof(Statistics));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "statistics:persist:read - pkey: %d, read: %d bytes", pkey, bytesRead);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "statistics - longest streak: %d, current streak: %d", statistics.longest_streak, statistics.current_streak);
}

void statistics_persist_write(int pkey) {
    int bytesWritten = persist_write_data(pkey, &statistics, sizeof(Statistics));
	int expectedBytesWritten = sizeof(Statistics);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "statistics:persist:write - pkey: %d, expected written: %d bytes. actual written: %d bytes", pkey, expectedBytesWritten, bytesWritten);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "statistics - longest streak: %d, current streak: %d", statistics.longest_streak, statistics.current_streak);
}

static void statistics_persist_reset(int pkey) {
	statistics.longest_streak = 0;
	statistics.current_streak = 0;
	statistics.last_date = 0;

	statistics_persist_write(pkey);
}

void statistics_persist_migrate(int pkey, int version) {
	switch (version) {
		case 0:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "statistics:persist:migrate - unknown version, resetting settings");
			statistics_persist_reset(pkey);
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "statistics:persist:migrate - already up to date");
			break;
	}
}

void update_statistics() {
	time_t today = time_start_of_today();

	if (statistics.last_date == today) {
		// Do nothing
	} else if (statistics.last_date == today - 24 * 60 * 60) {
		statistics.current_streak++;
	} else {
		statistics.current_streak = 1;
	}

	statistics.last_date = today;

	if (statistics.current_streak > statistics.longest_streak) {
		statistics.longest_streak = statistics.current_streak;
	}
}

char *statistics_tostring(char *output, int length) {
    snprintf(output, length, "Streak: %d Longest: %d", statistics.current_streak, statistics.longest_streak);

	return output;
}

int statistics_get_current_streak() {
    return statistics.current_streak;
}