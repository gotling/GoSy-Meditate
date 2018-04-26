#include <pebble.h>
#include "config.h"
#include "../common/entry.h"
#include "../common/tools.h"
#include "../common/storage.h"

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 5

static Window *window;
static TextLayer *header;
static MenuLayer *menu_layer;
void (*callback_function)(void);

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

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			menu_cell_basic_header_draw(ctx, cell_layer, "Notifications");
			break;
	}
}

static char subbuf[12];

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					format_time_long(subbuf, interval_settings.one);
					menu_cell_basic_draw(ctx, cell_layer, "First", subbuf, NULL);
					break;
				case 1:
					format_time_long(subbuf, interval_settings.two);
					menu_cell_basic_draw(ctx, cell_layer, "Second", subbuf, NULL);
					break;
				case 2:
					format_time_long(subbuf, interval_settings.three);
					menu_cell_basic_draw(ctx, cell_layer, "Third", subbuf, NULL);
					break;
				case 3:
					format_time_long(subbuf, interval_settings.four);
					menu_cell_basic_draw(ctx, cell_layer, "Last", subbuf, NULL);
					break;
				case 4:
					snprintf(subbuf, 7, "%d ms", interval_settings.length);
					menu_cell_basic_draw(ctx, cell_layer, "Vibration Length", subbuf, NULL);
					break;
			}
			break;
	}
}


static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					entry_init_time_zero_allowed("First", &interval_settings.one);
					break;
				case 1:
					entry_init_time_zero_allowed("Second", &interval_settings.two);
					break;
				case 2:
					entry_init_time_zero_allowed("Third", &interval_settings.three);
					break;
				case 3:
					entry_init_time_zero_allowed("Last", &interval_settings.four);
					break;
				case 4:
					entry_init_number("Vibration Length", "%d ms", 50, &interval_settings.length);
					break;
			}
			break;
	}
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	header = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text(header, "Configuration");
	text_layer_set_text_alignment(header, GTextAlignmentCenter);
	text_layer_set_font(header, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(header));
	
	menu_layer = menu_layer_create((GRect) { .origin = { 0, 40 }, .size = { bounds.size.w, MENU_HEIGHT } });
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	menu_layer_set_click_config_onto_window(menu_layer, window);
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

static void window_unload(Window *window) {
	persist_interval_write();
	
	text_layer_destroy(header);
	menu_layer_destroy(menu_layer);

	callback_function();

	window_destroy(window);
}

void interval_config_menu_init(void (*callback)(void)) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	const bool animated = true;
	callback_function = callback;
	window_stack_push(window, animated);
}

void interval_config_menu_deinit(void) {
	window_destroy(window);
}