#include "uhf_app_i.h"

static const char* uhf_file_header = "Flipper UHF device";
static const uint32_t uhf_file_version = 1;
static const uint8_t bank_data_start = 20;
static const uint8_t bank_data_length = 16;

// empty callback
void empty_rx_callback(UartIrqEvent event, uint8_t data, void* ctx) {
    UNUSED(event);
    UNUSED(data);
    UNUSED(ctx);
}

char* convertToHexString(const uint8_t* array, size_t length) {
    if(array == NULL || length == 0) {
        return NULL;
    }

    // Each byte takes 3 characters in the hex representation (2 characters + space), plus 1 for the null terminator
    size_t hexLength = (length * 3) + 1;

    char* hexArray = (char*)malloc(hexLength * sizeof(char));
    if(hexArray == NULL) {
        return NULL;
    }

    size_t index = 0;
    for(size_t i = 0; i < length; i++) {
        index += snprintf(&hexArray[index], hexLength - index, "%02x ", array[i]);
    }

    hexArray[hexLength - 1] = '\0';

    return hexArray;
}

bool uhf_save_read_data(UHFResponseData* uhf_response_data, Storage* storage, const char* filename) {
    if(!storage_dir_exists(storage, UHF_APPS_DATA_FOLDER)) {
        storage_simply_mkdir(storage, UHF_APPS_DATA_FOLDER);
    }
    if(!storage_dir_exists(storage, UHF_APPS_STORAGE_FOLDER)) {
        storage_simply_mkdir(storage, UHF_APPS_STORAGE_FOLDER);
    }

    FlipperFormat* file = flipper_format_file_alloc(storage);
    FuriString* temp_str = furi_string_alloc();
    // set file name
    furi_string_cat_printf(
        temp_str, "%s/%s%s", UHF_APPS_STORAGE_FOLDER, filename, UHF_FILE_EXTENSION);
    // open file
    if(!flipper_format_file_open_always(file, furi_string_get_cstr(temp_str))) return false;
    // write header
    if(!flipper_format_write_header_cstr(file, uhf_file_header, uhf_file_version)) return false;
    // write rfu data to file
    UHFData* rfu_data = uhf_response_data_get_uhf_data(uhf_response_data, 1);
    if(rfu_data->length) {
        if(!flipper_format_write_hex(
               file, "RFU", rfu_data->data + bank_data_start, bank_data_length))
            return false;
    } else {
        if(!flipper_format_write_hex(file, "RFU", UHF_BANK_DOES_NOT_EXIST, 1)) return false;
    }

    // write epc data to file
    UHFData* epc_data = uhf_response_data_get_uhf_data(uhf_response_data, 2);
    if(epc_data->length) {
        if(!flipper_format_write_hex(
               file, "EPC", epc_data->data + bank_data_start, bank_data_length))
            return false;
    } else {
        if(!flipper_format_write_hex(file, "EPC", UHF_BANK_DOES_NOT_EXIST, 1)) return false;
    }

    // write tid data to file
    UHFData* tid_data = uhf_response_data_get_uhf_data(uhf_response_data, 3);
    if(tid_data->length) {
        if(!flipper_format_write_hex(
               file, "TID", tid_data->data + bank_data_start, bank_data_length))
            return false;
    } else {
        if(!flipper_format_write_hex(file, "TID", UHF_BANK_DOES_NOT_EXIST, 1)) return false;
    }
    // write user data to file
    UHFData* user_data = uhf_response_data_get_uhf_data(uhf_response_data, 4);
    if(user_data->length) {
        if(!flipper_format_write_hex(
               file, "USER", user_data->data + bank_data_start, bank_data_length))
            return false;
    } else {
        if(!flipper_format_write_hex(file, "USER", UHF_BANK_DOES_NOT_EXIST, 1)) return false;
    }
    furi_string_free(temp_str);
    flipper_format_free(file);
    return true;
}

bool uhf_custom_event_callback(void* ctx, uint32_t event) {
    furi_assert(ctx);
    UHFApp* uhf_app = ctx;
    return scene_manager_handle_custom_event(uhf_app->scene_manager, event);
}

bool uhf_back_event_callback(void* ctx) {
    furi_assert(ctx);
    UHFApp* uhf_app = ctx;
    return scene_manager_handle_back_event(uhf_app->scene_manager);
}

void uhf_tick_event_callback(void* ctx) {
    furi_assert(ctx);
    UHFApp* uhf_app = ctx;
    scene_manager_handle_tick_event(uhf_app->scene_manager);
}

UHFApp* uhf_alloc() {
    UHFApp* uhf_app = (UHFApp*)malloc(sizeof(UHFApp));
    uhf_app->worker = (UHFWorker*)uhf_worker_alloc();
    uhf_app->view_dispatcher = view_dispatcher_alloc();
    uhf_app->scene_manager = scene_manager_alloc(&uhf_scene_handlers, uhf_app);
    view_dispatcher_enable_queue(uhf_app->view_dispatcher);
    view_dispatcher_set_event_callback_context(uhf_app->view_dispatcher, uhf_app);
    view_dispatcher_set_custom_event_callback(uhf_app->view_dispatcher, uhf_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        uhf_app->view_dispatcher, uhf_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        uhf_app->view_dispatcher, uhf_tick_event_callback, 100);

    // Open GUI record
    uhf_app->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(
        uhf_app->view_dispatcher, uhf_app->gui, ViewDispatcherTypeFullscreen);

    // Storage
    uhf_app->storage = furi_record_open(RECORD_STORAGE);

    // Open Notification record
    uhf_app->notifications = furi_record_open(RECORD_NOTIFICATION);

    // Submenu
    uhf_app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewMenu, submenu_get_view(uhf_app->submenu));

    // Popup
    uhf_app->popup = popup_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewPopup, popup_get_view(uhf_app->popup));

    // Loading
    uhf_app->loading = loading_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewLoading, loading_get_view(uhf_app->loading));

    // Text Input
    uhf_app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewTextInput, text_input_get_view(uhf_app->text_input));

    // Custom Widget
    uhf_app->widget = widget_alloc();
    view_dispatcher_add_view(
        uhf_app->view_dispatcher, UHFViewWidget, widget_get_view(uhf_app->widget));

    return uhf_app;
}

void uhf_free(UHFApp* uhf_app) {
    furi_assert(uhf_app);

    // Submenu
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewMenu);
    submenu_free(uhf_app->submenu);

    // Popup
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewPopup);
    popup_free(uhf_app->popup);

    // Loading
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewLoading);
    loading_free(uhf_app->loading);

    // TextInput
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewTextInput);
    text_input_free(uhf_app->text_input);

    // Custom Widget
    view_dispatcher_remove_view(uhf_app->view_dispatcher, UHFViewWidget);
    widget_free(uhf_app->widget);

    // Worker
    uhf_worker_stop(uhf_app->worker);
    uhf_worker_free(uhf_app->worker);

    // View Dispatcher
    view_dispatcher_free(uhf_app->view_dispatcher);

    // Scene Manager
    scene_manager_free(uhf_app->scene_manager);

    // GUI
    furi_record_close(RECORD_GUI);
    uhf_app->gui = NULL;

    // Storage
    furi_record_close(RECORD_STORAGE);
    uhf_app->storage = NULL;

    // Notifications
    furi_record_close(RECORD_NOTIFICATION);
    uhf_app->notifications = NULL;

    free(uhf_app);
}

static const NotificationSequence uhf_sequence_blink_start_cyan = {
    &message_blink_start_10,
    &message_blink_set_color_cyan,
    &message_do_not_reset,
    NULL,
};

static const NotificationSequence uhf_sequence_blink_stop = {
    &message_blink_stop,
    NULL,
};

void uhf_blink_start(UHFApp* uhf_app) {
    notification_message(uhf_app->notifications, &uhf_sequence_blink_start_cyan);
}

void uhf_blink_stop(UHFApp* uhf_app) {
    notification_message(uhf_app->notifications, &uhf_sequence_blink_stop);
}

void uhf_show_loading_popup(void* ctx, bool show) {
    UHFApp* uhf_app = ctx;
    TaskHandle_t timer_task = xTaskGetHandle(configTIMER_SERVICE_TASK_NAME);

    if(show) {
        // Raise timer priority so that animations can play
        vTaskPrioritySet(timer_task, configMAX_PRIORITIES - 1);
        view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewLoading);
    } else {
        // Restore default timer priority
        vTaskPrioritySet(timer_task, configTIMER_TASK_PRIORITY);
    }
}

int32_t uhf_app_main(void* ctx) {
    UNUSED(ctx);
    UHFApp* uhf_app = uhf_alloc();

    // enable 5v pin
    furi_hal_power_enable_otg();
    scene_manager_next_scene(uhf_app->scene_manager, UHFSceneVerify);
    view_dispatcher_run(uhf_app->view_dispatcher);

    // disable 5v pin
    furi_hal_power_disable_otg();

    // set uart callback to none
    furi_hal_uart_set_irq_cb(FuriHalUartIdUSART1, empty_rx_callback, NULL);

    // exit app
    uhf_free(uhf_app);
    return 0;
}