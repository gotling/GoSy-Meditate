#pragma once

typedef struct {
	int one;
	int two;
	int three;
	int four;
	int length;
} IntervalSettings;

extern IntervalSettings interval_settings;

void interval_persist_read(int pkey);
void interval_persist_write(int pkey);
void interval_persist_migrate(int pkey, int version);

char *interval_tostring(char *output, int length);
int interval_get_total_time();
int interval_menu_height();