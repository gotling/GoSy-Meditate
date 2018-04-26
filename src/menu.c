#include <pebble.h>
#include "interval/interval.h"
#include "interval/config.h"
#include "interval/config_menu.h"
#include "interval/statistics.h"
#include "common/tools.h"
#include "common/storage.h"

#define NUM_MENU_SECTIONS 0
#define NUM_FIRST_MENU_ITEMS 3

static Window *window;
static TextLayer *header;
static MenuLayer *menu_layer;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			return NUM_FIRST_MENU_ITEMS;
		default:
			return 0;
	}
}

static int16_t menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					return interval_menu_height();
			}
		default:
			return MENU_CELL_BASIC_HEIGHT;
	}
}

static char timebuf[7];
static char subbuf[40];

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					format_time(timebuf, interval_get_total_time());
					interval_tostring(subbuf, sizeof subbuf);
					menu_cell_basic_draw_multiline_with_extra_title(ctx, cell_layer, "Start", timebuf, subbuf, NULL);
					break;
				case 1:
					statistics_tostring(subbuf, sizeof subbuf);
					menu_cell_basic_draw_multiline(ctx, cell_layer, "Statistics", subbuf, NULL);
					break;
				case 2:
					menu_cell_basic_draw(ctx, cell_layer, "Configure", NULL, NULL);
					break;
			}
			break;
	}
}

static void reload_menu(void) {
	menu_layer_reload_data(menu_layer);

	menu_layer_set_selected_index(menu_layer, (MenuIndex) {.section = 0, .row = 0}, MenuRowAlignTop, true);
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					interval_init();
					break;
				case 1:
					break;
				case 2:
					interval_config_menu_init(&reload_menu);
					break;
			}
			break;
	}
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

	header = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text(header, "GoSy Meditate");
	text_layer_set_text_alignment(header, GTextAlignmentCenter);
	text_layer_set_font(header, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(header));
	
	menu_layer = menu_layer_create((GRect) { .origin = { 0, 40 }, .size = { bounds.size.w, MENU_HEIGHT } });
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_cell_height = menu_get_cell_height_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	menu_layer_set_click_config_onto_window(menu_layer, window);
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}


static void window_unload(Window *window) {
	text_layer_destroy(header);
	menu_layer_destroy(menu_layer);
}

void menu_init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	persist_read();
	
	const bool animated = true;
	window_stack_push(window, animated);
}

void menu_deinit(void) {
	window_destroy(window);
}