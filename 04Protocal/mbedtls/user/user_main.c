/******************************************************************************
* FileName: user_main.c
*
* Description: entry file of user application
*
* Modification history:
*     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#include "conn.h"


    #define SSID "TP-LINK-FD"
    #define PASSWORD "aaaaaaaa"


/* *
 * Note: check_memleak_debug_enable is a weak function inside SDK.
 * please copy following codes to user_main.c.
 */
#include "mem.h"

bool ICACHE_FLASH_ATTR check_memleak_debug_enable(void)
{
    return MEMLEAK_DEBUG_ENABLE;
}

LOCAL void ICACHE_FLASH_ATTR
wifi_handle_event_cb(System_Event_t *evt)
{
	void socket_test_init(void);
	void task_mdns_server(void);

	ESP_DBG("event %x\n", evt->event);

	switch (evt->event) {
		case EVENT_STAMODE_CONNECTED:
			break;
		case EVENT_STAMODE_DISCONNECTED:
			ESP_DBG("disconnect from ssid %s, reason %d\n",
			evt->event_info.disconnected.ssid,
			evt->event_info.disconnected.reason);
			break;
		case EVENT_STAMODE_AUTHMODE_CHANGE:
			ESP_DBG("mode: %d -> %d\n",
			evt->event_info.auth_change.old_mode,
			evt->event_info.auth_change.new_mode);
			break;
		case EVENT_STAMODE_GOT_IP:
			os_printf("heap %d\n",system_get_free_heap_size());			
			user_conn_init();
			break;
		case EVENT_SOFTAPMODE_STACONNECTED:
			ESP_DBG("station: " MACSTR "join, AID = %d\n",
			MAC2STR(evt->event_info.sta_connected.mac),
			evt->event_info.sta_connected.aid);
			break;
		case EVENT_SOFTAPMODE_STADISCONNECTED:
			ESP_DBG("station: " MACSTR "leave, AID = %d\n",
			MAC2STR(evt->event_info.sta_disconnected.mac),
			evt->event_info.sta_disconnected.aid);
			break;
		default:
			break;
	}
}

void user_rf_pre_init(void)
{
}
void ICACHE_FLASH_ATTR
user_set_station_config(void)
{
   // Wifi configuration 
   char ssid[32] = SSID; 
   char password[64] = PASSWORD; 
   struct station_config stationConf; 
   
   os_memset(stationConf.ssid, 0, 32);
   os_memset(stationConf.password, 0, 64);
   //need not mac address
   stationConf.bssid_set = 0; 
   
   //Set ap settings 
   os_memcpy(&stationConf.ssid, ssid, 32); 
   os_memcpy(&stationConf.password, password, 64); 
   wifi_station_set_config(&stationConf); 

}

void user_init(void)
{
	ESP_DBG("SDK version:%s\n", system_get_sdk_version());
	system_print_meminfo();
	wifi_set_event_handler_cb(wifi_handle_event_cb);
	wifi_set_opmode(STATION_MODE);
    user_set_station_config();
	wifi_set_sleep_type(NONE_SLEEP_T);
}
