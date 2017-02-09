#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define ESP_DEBUG
//#define USE_OPTIMIZE_PRINTF
//#define USE_INTER_LED

#ifdef USE_INTER_LED //使用自带LED，无法使用TX输出
#undef ESP_DEBUG
#endif

#ifdef ESP_DEBUG
#define ESP_DBG os_printf
#else
#define ESP_DBG
#endif

//定义SmartConfig设置键为 GPIO0
#define CONFIG_KEY_NUM 1
#define CONFIG_KEY_IO_MUX PERIPHS_IO_MUX_GPIO0_U
#define CONFIG_KEY_IO_NUM 0
#define CONFIG_KEY_IO_FUNC FUNC_GPIO0

#ifdef USE_INTER_LED
//定义WIFI指示灯为 GPIO1
#define WIFI_LED_IO_MUX PERIPHS_IO_MUX_U0TXD_U //PERIPHS_IO_MUX_GPIO0_U
#define WIFI_LED_IO_NUM 1 //0
#define WIFI_LED_IO_FUNC FUNC_GPIO1 //FUNC_GPIO0
//定义LED灯为 GPIO1
#define USER_LED_IO_MUX PERIPHS_IO_MUX_U0TXD_U
#define USER_LED_IO_NUM 1
#define USER_LED_IO_FUNC FUNC_GPIO1
#define LEDON 0
#define LEDOFF 1
#else
//定义WIFI指示灯为 GPIO2
#define WIFI_LED_IO_MUX PERIPHS_IO_MUX_GPIO2_U
#define WIFI_LED_IO_NUM 2
#define WIFI_LED_IO_FUNC FUNC_GPIO2
//定义LED灯为 GPIO2
#define USER_LED_IO_MUX PERIPHS_IO_MUX_GPIO2_U
#define USER_LED_IO_NUM 2
#define USER_LED_IO_FUNC FUNC_GPIO2
#define LEDON 1
#define LEDOFF 0
#endif

#define SERVER_IP 0

//#define SSID "dlink-2AA0"
//#define PASSWORD "12345678"

#if 0
#define SERVER_PORT 443
#define SERVER_DOMAIN "192.168.100.104"
#define HTTP_REQUEST "GET /opensearch.xml HTTP/1.1\r\nHost: github.com\r\nAccept: */*\r\nConnection: keep-alive\r\nUser-Agent: esp8266/1.0\r\n\r\n"
#define REQUEST_INTERVAL 10000 //毫秒
#else
#define SERVER_PORT 8443
#define SERVER_DOMAIN "portal.okidokeys.com"
#define HTTP_REQUEST "{\"path\": \"/v1/ping/\", \"method\": \"GET\"}\r\n";
#define REQUEST_INTERVAL 10000 //毫秒

#endif

#if 0

#define SERVER_PORT 8883
#define SERVER_DOMAIN "A1QQ280DDHPJDD.iot.us-east-1.amazonaws.com"

#endif
#endif

