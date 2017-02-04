#include <pebble.h>

typedef enum ActivityState {
    STATE_STARTED,
    STATE_STOPPED
} ActivityState;

static Window *s_window;

static TextLayer *s_time_text_layer;
static TextLayer *s_day_name_text_layer;
static TextLayer *s_date_week_time_layer;
static Layer *s_bt_image_layer;
static Layer *s_status_image_layer;
static GBitmap *s_status_image;
static GBitmap *s_bt_image;
static Layer *s_progress_layer;
static GFont s_banana_brick_font_42;
static bool bluetooth;

static ActivityState s_app_state = STATE_STARTED;
static uint16_t s_curr_hr = 0;



static void update_time(struct tm *tick_time) {
    // Create a long-lived buffer
    static char time_buffer[] = "00:00";
    static char day_name_buffer[] = "Wednesday";
    static char date_week_buffer[] = "00 Mon YEAR, wk no";

    // Write the current hours and minutes into the buffer
    if (clock_is_24h_style() == true) {
        // Use 24 hour format
        strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
    } else {
        // Use 12 hour format
        strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
    }

    strftime(day_name_buffer, sizeof("Wednesday"), "%A", tick_time);
    strftime(date_week_buffer, sizeof("00 Mon YEAR, wk no"), "%d %b %Y, wk %W", tick_time);

    // Display this time on the TextLayer
    text_layer_set_text(s_time_text_layer, time_buffer);
    text_layer_set_text(s_day_name_text_layer, day_name_buffer);
    text_layer_set_text(s_date_week_time_layer, date_week_buffer);
}

static void prv_on_activity_tick(struct tm *tick_time, TimeUnits units_changed) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "prv_on_activity_tick: start, Heap Available: %d", heap_bytes_free());
    update_time(tick_time);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "prv_on_activity_tick: end, Heap Available: %d", heap_bytes_free());
}

static void update_bt_image_layer_proc(Layer *layer, GContext *ctx) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "update_bt_image_layer_proc: start, Heap Available: %d", heap_bytes_free());

    GBitmap *old_s_bt_image = s_bt_image;

    if (bluetooth) {
        s_bt_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_ON);
    } else {
        s_bt_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BT_OFF);
    }

    graphics_draw_bitmap_in_rect(ctx, s_bt_image, layer_get_bounds(s_bt_image_layer));

    // destroy old image to free up heap memory
    gbitmap_destroy(old_s_bt_image);
    APP_LOG(APP_LOG_LEVEL_ERROR, "update_bt_image_layer_proc: end, Heap Available: %d", heap_bytes_free());
}

void bluetooth_callback(bool connected) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "bluetooth_callback: start");
    bluetooth = connected;

    layer_mark_dirty(s_bt_image_layer);
    APP_LOG(APP_LOG_LEVEL_ERROR, "bluetooth_callback: end");
}

static void update_status_image_layer_proc(Layer *layer, GContext *ctx) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "update_status_image_layer_proc: start, Heap Available: %d", heap_bytes_free());

    GBitmap *old_s_status_image = s_status_image;

    if (s_curr_hr < 110) {
        s_status_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_OK);
    } else {
        s_status_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_VIBRATE);
    }

    APP_LOG(APP_LOG_LEVEL_ERROR, "update_status_image_layer_proc: before draw, Heap Available: %d", heap_bytes_free());
    graphics_draw_bitmap_in_rect(ctx, s_status_image, layer_get_bounds(s_status_image_layer));
    APP_LOG(APP_LOG_LEVEL_ERROR, "update_status_image_layer_proc: after draw, Heap Available: %d", heap_bytes_free());

    // destroy old image to free up heap memory
    gbitmap_destroy(old_s_status_image);
    APP_LOG(APP_LOG_LEVEL_ERROR, "update_status_image_layer_proc: end, Heap Available: %d", heap_bytes_free());
}

static void update_progress_layer_proc(Layer *layer, GContext *ctx) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "update_progress_layer_proc: start, Heap Available: %d", heap_bytes_free());

    GRect bounds = layer_get_bounds(layer);

    uint16_t progress = 0;
    if(s_curr_hr > 70 && s_curr_hr < 80 ) {
        progress = 25;
    } else if(s_curr_hr > 80 && s_curr_hr < 90 ) {
        progress = 50;
    } else if(s_curr_hr > 90 && s_curr_hr < 100 ) {
        progress = 75;
    } else if(s_curr_hr > 100) {
        progress = 75;
    }

    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, (GRect) {.origin = {2, 90}, .size = {100, 3}}, 0, GCornerNone);
    graphics_fill_rect(ctx, (GRect) {.origin = {2, 108}, .size = {100, 3}}, 0, GCornerNone);

    graphics_fill_rect(ctx, (GRect) {.origin = {2, 85}, .size = {3, 5}}, 0, GCornerNone);
    graphics_fill_rect(ctx, (GRect) {.origin = {27, 87}, .size = {3, 3}}, 0, GCornerNone);
    graphics_fill_rect(ctx, (GRect) {.origin = {52, 85}, .size = {3, 5}}, 0, GCornerNone);
    graphics_fill_rect(ctx, (GRect) {.origin = {77, 87}, .size = {3, 3}}, 0, GCornerNone);
    graphics_fill_rect(ctx, (GRect) {.origin = {99, 85}, .size = {3, 5}}, 0, GCornerNone);

    graphics_fill_rect(ctx, (GRect) {.origin = {2, 111}, .size = {3, 5}}, 0, GCornerNone);
    graphics_fill_rect(ctx, (GRect) {.origin = {27, 111}, .size = {3, 3}}, 0, GCornerNone);
    graphics_fill_rect(ctx, (GRect) {.origin = {52, 111}, .size = {3, 5}}, 0, GCornerNone);
    graphics_fill_rect(ctx, (GRect) {.origin = {77, 111}, .size = {3, 3}}, 0, GCornerNone);
    graphics_fill_rect(ctx, (GRect) {.origin = {99, 111}, .size = {3, 5}}, 0, GCornerNone);

    graphics_fill_rect(ctx, (GRect) {.origin = {2, 93}, .size = {progress, 15}}, 0, GCornerNone);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "update_progress_layer_proc: end, Heap Available: %d", heap_bytes_free());
}

static void prv_on_health_data(HealthEventType type, void *context) {
    // If the update was from the Heart Rate Monitor, update it
    if (type == HealthEventHeartRateUpdate) {
        s_curr_hr = (int16_t) health_service_peek_current_value(HealthMetricHeartRateRawBPM);
    }
}

static void prv_start_activity(void) {
    // Update application state
    s_app_state = STATE_STARTED;

    // Set min heart rate sampling period (i.e. fastest sampling rate)
    #if PBL_API_EXISTS(health_service_set_heart_rate_sample_period)
    health_service_set_heart_rate_sample_period(1);
    #endif

    // Subscribe to tick handler to update display
    tick_timer_service_subscribe(SECOND_UNIT, prv_on_activity_tick);

    // Subscribe to health handler
    health_service_events_subscribe(prv_on_health_data, NULL);
}

static void prv_end_activity(void) {
    // Update application state
    s_app_state = STATE_STOPPED;

    // Set default heart rate sampling period
    #if PBL_API_EXISTS(health_service_set_heart_rate_sample_period)
    health_service_set_heart_rate_sample_period(0);
    #endif

    // Unsubscribe from tick handler
    tick_timer_service_unsubscribe();

    // Unsubscribe from health handler
    health_service_events_unsubscribe();
}

static void prv_select_click_handler(ClickRecognizerRef recognizer, void *context) {
    switch (s_app_state) {
        case STATE_STOPPED:
            // Display activity
            prv_start_activity();
            break;
        case STATE_STARTED:
            // Display first screen
            prv_end_activity();
            break;
    }
}

static void prv_click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, prv_select_click_handler);
}

static void prv_window_load(Window *window) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "window_load: start");

    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_day_name_text_layer = text_layer_create((GRect) {.origin = {0, 13}, .size = {bounds.size.w - 30, 25}});
    text_layer_set_background_color(s_day_name_text_layer, GColorClear);
    text_layer_set_text_color(s_day_name_text_layer, GColorBlack);
    text_layer_set_font(s_day_name_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
    text_layer_set_text_alignment(s_day_name_text_layer, GTextAlignmentCenter);

    s_banana_brick_font_42 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_BANANA_BRICK_36));
    s_time_text_layer = text_layer_create((GRect) {.origin = {0, 35}, .size = {bounds.size.w - 30, 43}});
    text_layer_set_background_color(s_time_text_layer, GColorClear);
    text_layer_set_text_color(s_time_text_layer, GColorBlack);
    text_layer_set_font(s_time_text_layer, s_banana_brick_font_42);
    text_layer_set_text_alignment(s_time_text_layer, GTextAlignmentCenter);

    s_date_week_time_layer = text_layer_create((GRect) {.origin = {0, 130}, .size = {bounds.size.w, 20}});
    text_layer_set_background_color(s_date_week_time_layer, GColorClear);
    text_layer_set_text_color(s_date_week_time_layer, GColorBlack);
    text_layer_set_font(s_date_week_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
    text_layer_set_text_alignment(s_date_week_time_layer, GTextAlignmentCenter);

    s_bt_image_layer = layer_create((GRect) {.origin = {bounds.size.w - 30, 20}, .size = {22, 52}});
    layer_set_update_proc(s_bt_image_layer, update_bt_image_layer_proc);

    s_status_image_layer = layer_create((GRect) {.origin = {bounds.size.w - 40, 80}, .size = {40, 40}});
    layer_set_update_proc(s_status_image_layer, update_status_image_layer_proc);

    s_progress_layer = layer_create((GRect) {.origin = {0, 0}, .size = {bounds.size.w, bounds.size.h}});
    layer_set_update_proc(s_progress_layer, update_progress_layer_proc);

    layer_add_child(window_layer, text_layer_get_layer(s_day_name_text_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_time_text_layer));
    layer_add_child(window_layer, text_layer_get_layer(s_date_week_time_layer));
    layer_add_child(window_layer, s_bt_image_layer);
    layer_add_child(window_layer, s_status_image_layer);
    layer_add_child(window_layer, s_progress_layer);

    bool is_connected = bluetooth_connection_service_peek();
    bluetooth_callback(is_connected);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "window_load: end");
}

static void prv_window_unload(Window *window) {
    fonts_unload_custom_font(s_banana_brick_font_42);

    text_layer_destroy(s_time_text_layer);
    text_layer_destroy(s_day_name_text_layer);
    text_layer_destroy(s_date_week_time_layer);

    gbitmap_destroy(s_status_image);
    gbitmap_destroy(s_bt_image);

    layer_destroy(s_bt_image_layer);
    layer_destroy(s_status_image_layer);
    layer_destroy(s_progress_layer);
}

static void prv_init(void) {
    s_window = window_create();
    window_set_click_config_provider(s_window, prv_click_config_provider);
    window_set_window_handlers(s_window, (WindowHandlers) {
            .load = prv_window_load,
            .unload = prv_window_unload,
    });

    window_stack_push(s_window, true);

    // Update time immediately to avoid flash of "timeless" clock
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    update_time(t);

    bluetooth_connection_service_subscribe(bluetooth_callback);
}

static void prv_deinit(void) {
    window_destroy(s_window);

    // Reset Heart Rate Sampling
    #if PBL_API_EXISTS(health_service_set_heart_rate_sample_period)
    health_service_set_heart_rate_sample_period(0);
    #endif

    bluetooth_connection_service_unsubscribe();
}

int main(void) {
    prv_init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

    app_event_loop();
    prv_deinit();
}
