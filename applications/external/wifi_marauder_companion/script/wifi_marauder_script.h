/*
 * ----------------------------------------------------------------------------------------------------
 * STEPS TO ADD A NEW STAGE:
 * 
 * wifi_marauder_script.h
 * - Complement WifiMarauderScriptStageType enum with new stage
 * - Create struct WifiMarauderScriptStage???? for the new stage
 * 
 * wifi_marauder_script.c
 * - Create function "WifiMarauderScriptStage????* _wifi_marauder_script_get_stage_????(cJSON *stages)"
 * - Change _wifi_marauder_script_load_stages() to load new stage
 * - Add case to free memory in wifi_marauder_script_free()
 * 
 * wifi_marauder_script_executor.c
 * - Create function "void _wifi_marauder_script_execute_????(WifiMarauderScriptStage????* stage)"
 * - Add case in wifi_marauder_script_execute_stage()
 * 
 * ----------------------------------------------------------------------------------------------------
 * IMPLEMENTED STAGES (In order of execution):
 * - Scan
 * - Select
 * - Deauth
 * - Sniff PMKID
 * - Beacon List
 * ----------------------------------------------------------------------------------------------------
 * SCRIPT SYNTAX:
 * {
 *     "meta": {
 *         "description": "My script",
 *         "repeat": times the script will repeat
 *     },
 *     "stages": {
 *         "scan": {
 *             "type": "ap" | "station",
 *             "timeout": seconds,
 *             "channel": 1-11
 *         },
 *         "select": {
 *             "type": "ap" | "station" | "ssid",
 *             "filter": "all" | "contains \"{SSID fragment}\" or equals \"{SSID}\" or ..."
 *         },
 *         "deauth": {
 *             "timeout": seconds
 *         },
 *         "sniffPmkid": {
 *             "forceDeauth": true | false,
 *             "channel": 1-11,
 *             "timeout": seconds
 *         },
 *         "beaconlist": {
 *             "ssids": [
 *                 "SSID 1",
 *                 "SSID 2",
 *                 "SSID 3"
 *             ],
 *             "timeout": seconds
 *         }
 *     }
 * }
 * ----------------------------------------------------------------------------------------------------
 */

#pragma once

#include <storage/storage.h>
#include "cJSON.h"

typedef enum {
    WifiMarauderScriptStageTypeScan,
    WifiMarauderScriptStageTypeSelect,
    WifiMarauderScriptStageTypeDeauth,
    WifiMarauderScriptStageTypeSniffPmkid,
    WifiMarauderScriptStageTypeBeaconList,
} WifiMarauderScriptStageType;

typedef enum {
    WifiMarauderScriptScanTypeAp,
    WifiMarauderScriptScanTypeStation
} WifiMarauderScriptScanType;

typedef enum {
    WifiMarauderScriptSelectTypeAp,
    WifiMarauderScriptSelectTypeStation,
    WifiMarauderScriptSelectTypeSsid
} WifiMarauderScriptSelectType;

// Stages
typedef struct WifiMarauderScriptStage {
    WifiMarauderScriptStageType type;
    void* stage;
    struct WifiMarauderScriptStage* next_stage;
} WifiMarauderScriptStage;

typedef struct WifiMarauderScriptStageScan {
    WifiMarauderScriptScanType type;
    int channel;
    int timeout;
} WifiMarauderScriptStageScan;

typedef struct WifiMarauderScriptStageSelect {
    WifiMarauderScriptSelectType type;
    char* filter;
    // TODO: Implement a feature to not select the same items in the next iteration of the script
    bool allow_repeat;
} WifiMarauderScriptStageSelect;

typedef struct WifiMarauderScriptStageDeauth {
    int timeout;
} WifiMarauderScriptStageDeauth;

typedef struct WifiMarauderScriptStageSniffPmkid {
    bool force_deauth;
    int channel;
    int timeout;
} WifiMarauderScriptStageSniffPmkid;

typedef struct WifiMarauderScriptStageBeaconList {
    char** ssids;
    int ssid_count;
    int timeout;
} WifiMarauderScriptStageBeaconList;

// Script
typedef struct WifiMarauderScript {
    char* description;
    WifiMarauderScriptStage* first_stage;
    int repeat;
} WifiMarauderScript;

WifiMarauderScript* wifi_marauder_script_alloc();
WifiMarauderScript* wifi_marauder_script_parse_raw(const char* script_raw);
WifiMarauderScript* wifi_marauder_script_parse_file(const char* file_path, Storage* storage);
void wifi_marauder_script_free(WifiMarauderScript* script);
