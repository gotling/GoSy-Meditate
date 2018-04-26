#pragma once

typedef struct {
    int longest_streak;
    int current_streak;
    int last_date;
} Statistics;

extern Statistics statistics;

void statistics_persist_read(int pkey);
void statistics_persist_write(int pkey);
void statistics_persist_migrate(int pkey, int version);

void update_statistics();

char *statistics_tostring(char *output, int length);
int statistics_get_current_streak();