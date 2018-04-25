#include <pebble.h>
#include "storage.h"
#include "interval/config.h"

#define STORAGE_VERSION_PKEY 30
#define INTERVAL_PKEY 31

#define CURRENT_STORAGE_VERSION 1

static int persist_version(void) {
    uint32_t version = persist_read_int(STORAGE_VERSION_PKEY);

    return version;
}

static void persist_version_set(int version) {
    persist_write_int(STORAGE_VERSION_PKEY, version);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist:version:set - pkey: %d, version: %d", STORAGE_VERSION_PKEY, version);
}

static int persist_current_version(void) {
    return CURRENT_STORAGE_VERSION;
}

static void persist_migrate(void) {
    int version = persist_version();
    if (version == persist_current_version()) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "persist:migrate - already up to date");
        return;
    }

    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist:migrate - v%d to v%d", version, persist_current_version());
    interval_persist_migrate(INTERVAL_PKEY, version);

    persist_version_set(persist_current_version());
}

void persist_interval_write(void) {
    interval_persist_write(INTERVAL_PKEY);
}

void persist_read(void) {
    persist_migrate();

    interval_persist_read(INTERVAL_PKEY);
}