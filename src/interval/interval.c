#include <pebble.h>
#include "config.h"
#include "statistics.h"
#include "../common/storage.h"
#include "../common/tools.h"

enum _activity { MEDITATE, PAUSED };

static struct IntervalUi {
	Window *window;
	TextLayer *top_text;
	TextLayer *middle_text;
	TextLayer *time_text;
	BitmapLayer *image;
} ui;

static struct IntervalState {
	bool active;
	uint16_t total_time;
	enum _activity activity;
	enum _activity paused_activity;
} state;

static void update_time() {
	state.total_time++;
}

static char timebuf1[7];

// Every second
static void update_time_ui() {
	text_layer_set_text(ui.time_text, format_time(timebuf1, state.total_time));
}

// When changing from running to paused or vise verse
static void update_ui() {
	switch (state.activity) {
		case MEDITATE:
			text_layer_set_text(ui.top_text, "MEDITATE");
			break;
		case PAUSED:
			text_layer_set_text(ui.top_text, "PAUSED");
			break;
		default:
			break;
	}
}

static void timer_callback(struct tm *tick_time, TimeUnits units_changed) {
	if (state.total_time == interval_settings.one) {
		vibrate(interval_settings.length, 1);
	} else if (state.total_time == interval_settings.two) {
		vibrate(interval_settings.length, 2);
	} else if (state.total_time == interval_settings.three) {
		vibrate(interval_settings.length, 3);
	} else if (state.total_time == interval_settings.four) {
		vibes_enqueue_custom_pattern(end_vibration);
	}

	update_time();
	update_time_ui();
	update_ui();
}

static void start() {
	if (state.activity == PAUSED) {
		state.activity = state.paused_activity;
	}
	
	state.active = true;
	
	vibes_short_pulse();
	
	update_ui();
	update_time_ui();
	
	tick_timer_service_subscribe(SECOND_UNIT, &timer_callback);
}

static void pause() {
	tick_timer_service_unsubscribe();
	
	state.active = false;
	state.paused_activity = state.activity;
	state.activity = PAUSED;
	
	update_ui();
}

static void reset() {
	tick_timer_service_unsubscribe();
	
	state.active = false;
	state.total_time = 0;
	
	update_ui();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (state.activity != PAUSED) {
		pause();
	} else {
		start();
	}
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	ui.top_text = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text_alignment(ui.top_text, GTextAlignmentCenter);
	text_layer_set_font(ui.top_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(ui.top_text));
	
	ui.middle_text = text_layer_create((GRect) { .origin = { 0, 32 }, .size = { bounds.size.w, 26 } });
	text_layer_set_text_alignment(ui.middle_text, GTextAlignmentCenter);
	text_layer_set_overflow_mode(ui.top_text, GTextOverflowModeWordWrap);
	text_layer_set_font(ui.middle_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(ui.middle_text));

	ui.time_text = text_layer_create((GRect) { .origin = { 0, 58 }, .size = { bounds.size.w, 52 } });
	text_layer_set_text_alignment(ui.time_text, GTextAlignmentCenter);
	text_layer_set_font(ui.time_text, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
	layer_add_child(window_layer, text_layer_get_layer(ui.time_text));
	
	reset();
	start();
}

static void window_unload(Window *window) {
	reset();
	update_statistics();
	persist_statistics_write();
	
	text_layer_destroy(ui.top_text);
	text_layer_destroy(ui.middle_text);
	text_layer_destroy(ui.time_text);
	
	window_destroy(window);
}

void interval_init(void) {
	ui.window = window_create();
	window_set_click_config_provider(ui.window, click_config_provider);
	window_set_window_handlers(ui.window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

void interval_deinit(void) {
	window_destroy(ui.window);
}