// #include "../cfw_app.h"

// enum VarItemListIndex {
    // VarItemListIndexBatteryIcon,
    // VarItemListIndexStatusIcons,
    // VarItemListIndexBarBorders,
    // VarItemListIndexBarBackground,
// };

// void cfw_app_scene_interface_statusbar_var_item_list_callback(void* context, uint32_t index) {
    // CfwApp* app = context;
    // view_dispatcher_send_custom_event(app->view_dispatcher, index);
// }

// const char* const battery_icon_names[] =
    // {"OFF", "Bar", "%", "Inv. %", "Retro 3", "Retro 5", "Bar %"};
// static void cfw_app_scene_interface_statusbar_battery_icon_changed(VariableItem* item) {
    // CfwApp* app = variable_item_get_context(item);
    // uint8_t index = variable_item_get_current_value_index(item);
    // variable_item_set_current_value_text(item, battery_icon_names[index]);
    // CFW_SETTINGS()->battery_icon = index;
    // app->save_settings = true;
// }

// static void cfw_app_scene_interface_statusbar_status_icons_changed(VariableItem* item) {
    // CfwApp* app = variable_item_get_context(item);
    // bool value = variable_item_get_current_value_index(item);
    // variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    // CFW_SETTINGS()->status_icons = value;
    // app->save_settings = true;
// }

// static void cfw_app_scene_interface_statusbar_bar_borders_changed(VariableItem* item) {
    // CfwApp* app = variable_item_get_context(item);
    // bool value = variable_item_get_current_value_index(item);
    // variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    // CFW_SETTINGS()->bar_borders = value;
    // app->save_settings = true;
// }

// static void cfw_app_scene_interface_statusbar_bar_background_changed(VariableItem* item) {
    // CfwApp* app = variable_item_get_context(item);
    // bool value = variable_item_get_current_value_index(item);
    // variable_item_set_current_value_text(item, value ? "ON" : "OFF");
    // CFW_SETTINGS()->bar_background = value;
    // app->save_settings = true;
// }

// void cfw_app_scene_interface_statusbar_on_enter(void* context) {
    // CfwApp* app = context;
    // CfwSettings* cfw_settings = CFW_SETTINGS();
    // VariableItemList* var_item_list = app->var_item_list;
    // VariableItem* item;

    // item = variable_item_list_add(
        // var_item_list,
        // "Battery Icon",
        // BatteryIconCount,
        // cfw_app_scene_interface_statusbar_battery_icon_changed,
        // app);
    // variable_item_set_current_value_index(item, cfw_settings->battery_icon);
    // variable_item_set_current_value_text(item, battery_icon_names[cfw_settings->battery_icon]);

    // item = variable_item_list_add(
        // var_item_list,
        // "Status Icons",
        // 2,
        // cfw_app_scene_interface_statusbar_status_icons_changed,
        // app);
    // variable_item_set_current_value_index(item, cfw_settings->status_icons);
    // variable_item_set_current_value_text(item, cfw_settings->status_icons ? "ON" : "OFF");

    // item = variable_item_list_add(
        // var_item_list,
        // "Bar Borders",
        // 2,
        // cfw_app_scene_interface_statusbar_bar_borders_changed,
        // app);
    // variable_item_set_current_value_index(item, cfw_settings->bar_borders);
    // variable_item_set_current_value_text(item, cfw_settings->bar_borders ? "ON" : "OFF");

    // item = variable_item_list_add(
        // var_item_list,
        // "Bar Background",
        // 2,
        // cfw_app_scene_interface_statusbar_bar_background_changed,
        // app);
    // variable_item_set_current_value_index(item, cfw_settings->bar_background);
    // variable_item_set_current_value_text(item, cfw_settings->bar_background ? "ON" : "OFF");

    // variable_item_list_set_enter_callback(
        // var_item_list, cfw_app_scene_interface_statusbar_var_item_list_callback, app);

    // variable_item_list_set_selected_item(
        // var_item_list,
        // scene_manager_get_scene_state(app->scene_manager, CfwAppSceneInterfaceStatusbar));

    // view_dispatcher_switch_to_view(app->view_dispatcher, CfwAppViewVarItemList);
// }

// bool cfw_app_scene_interface_statusbar_on_event(void* context, SceneManagerEvent event) {
    // CfwApp* app = context;
    // bool consumed = false;

    // if(event.type == SceneManagerEventTypeCustom) {
        // scene_manager_set_scene_state(
            // app->scene_manager, CfwAppSceneInterfaceStatusbar, event.event);
        // consumed = true;
        // switch(event.event) {
        // default:
            // break;
        // }
    // }

    // return consumed;
// }

// void cfw_app_scene_interface_statusbar_on_exit(void* context) {
    // CfwApp* app = context;
    // variable_item_list_reset(app->var_item_list);
// }
