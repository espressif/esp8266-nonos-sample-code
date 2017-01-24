/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: espconn_encry.c
 *
 * Description: data encrypt interface
 *
 * Modification history:
 *     2014/3/31, v1.0 create this file.
*******************************************************************************/

#include "lwip/netif.h"
#include "lwip/inet.h"
#include "netif/etharp.h"
#include "lwip/tcp.h"
#include "lwip/ip.h"
#include "lwip/init.h"
#include "ets_sys.h"
#include "os_type.h"

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

#if !defined(ESPCONN_MBEDTLS)

#include "sys/espconn_mbedtls.h"
#include "user_interface.h"

ssl_opt ssl_option = {
		{NULL, ESPCONN_SECURE_DEFAULT_SIZE, 0, false, 0, false},
		{NULL, ESPCONN_SECURE_DEFAULT_SIZE, 0, false, 0, false},
		0
};

unsigned int def_private_key_len = 0;
unsigned char *def_private_key = NULL;
unsigned char *def_certificate = NULL;
unsigned int def_certificate_len = 0;
unsigned int max_content_len = ESPCONN_SECURE_DEFAULT_SIZE;
/******************************************************************************
 * FunctionName : espconn_encry_connect
 * Description  : The function given as the connect
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_secure_connect(struct espconn *espconn)
{	
	struct ip_addr ipaddr;
	struct ip_info ipinfo;
	uint8 connect_status = 0;
	uint16 current_size = 0;
	if (espconn == NULL || espconn ->type != ESPCONN_TCP)
		return ESPCONN_ARG;
	
	if (wifi_get_opmode() == ESPCONN_STA){
		wifi_get_ip_info(STA_NETIF, &ipinfo);
		if (ipinfo.ip.addr == 0) {
			return ESPCONN_RTE;
		}
	} else if (wifi_get_opmode() == ESPCONN_AP) {
		wifi_get_ip_info(AP_NETIF, &ipinfo);
		if (ipinfo.ip.addr == 0) {
			return ESPCONN_RTE;
		}
	} else if (wifi_get_opmode() == ESPCONN_AP_STA) {
		IP4_ADDR(&ipaddr, espconn->proto.tcp->remote_ip[0],
				espconn->proto.tcp->remote_ip[1],
				espconn->proto.tcp->remote_ip[2],
				espconn->proto.tcp->remote_ip[3]);
		ipaddr.addr <<= 8;
		wifi_get_ip_info(AP_NETIF, &ipinfo);
		ipinfo.ip.addr <<= 8;
		espconn_printf("softap_addr = %x, remote_addr = %x\n", ipinfo.ip.addr, ipaddr.addr);

		if (ipaddr.addr != ipinfo.ip.addr) {
			connect_status = wifi_station_get_connect_status();
			if (connect_status == STATION_GOT_IP) {
				wifi_get_ip_info(STA_NETIF, &ipinfo);
				if (ipinfo.ip.addr == 0)
					return ESPCONN_RTE;
			} else if (connect_status == STATION_IDLE) {
				return ESPCONN_RTE;
			} else {
				return connect_status;
			}
		}
	}
	current_size = espconn_secure_get_size(ESPCONN_CLIENT);
	current_size += ESPCONN_SECURE_DEFAULT_HEAP;
//	ssl_printf("heap_size %d %d\n", system_get_free_heap_size(), current_size);
	if (system_get_free_heap_size() <= current_size)
		return ESPCONN_MEM;

	return espconn_ssl_client(espconn);
}

/******************************************************************************
 * FunctionName : espconn_encry_disconnect
 * Description  : The function given as the disconnect
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_secure_disconnect(struct espconn *espconn)
{
	espconn_msg *pnode = NULL;
	bool value = false;
	if (espconn == NULL)
		return ESPCONN_ARG;

	value = espconn_find_connection(espconn, &pnode);
	if (value){
		espconn_ssl_disconnect(pnode);
		return ESPCONN_OK;
	}
	else
		return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_encry_sent
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 * 				  psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR
espconn_secure_sent(struct espconn *espconn, uint8 *psent, uint16 length)
{
	espconn_msg *pnode = NULL;
	bool value = false;
	if (espconn == NULL)
		return ESPCONN_ARG;

	espconn ->state = ESPCONN_WRITE;
	value = espconn_find_connection(espconn, &pnode);
	if (value){
		pmbedtls_msg pssl = NULL;
		pssl = pnode->pssl;
		if (pssl->SentFnFlag){
			pssl->SentFnFlag = false;
			espconn_ssl_sent(pnode, psent, length);
			return ESPCONN_OK;
		}else
			return ESPCONN_INPROGRESS;
	}
	else
		return ESPCONN_ARG;
}

/******************************************************************************
 * FunctionName : espconn_secure_send
 * Description  : send data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 *                psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/

sint8 espconn_secure_send(struct espconn *espconn, uint8 *psent, uint16 length) __attribute__((alias("espconn_secure_sent")));

sint8 ICACHE_FLASH_ATTR
espconn_secure_accept(struct espconn *espconn)
{
	if (espconn == NULL || espconn ->type != ESPCONN_TCP)
		return ESPCONN_ARG;

	return espconn_ssl_server(espconn);
}

/******************************************************************************
 * FunctionName : espconn_secure_set_size
 * Description  : set the buffer size for client or server
 * Parameters   : level -- set for client or server
 * 				  1: client,2:server,3:client and server
 * 				  size -- buffer size
 * Returns      : true or false
*******************************************************************************/
bool ICACHE_FLASH_ATTR espconn_secure_set_size(uint8 level, uint16 size)
{
	if (level >= ESPCONN_MAX || level <= ESPCONN_IDLE)
		return false;

	if (size > ESPCONN_SECURE_MAX_SIZE || size < ESPCONN_SECURE_DEFAULT_SIZE)
		return false;

	max_content_len = size;
	return true;
}

/******************************************************************************
 * FunctionName : espconn_secure_get_size
 * Description  : get buffer size for client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 * Returns      : buffer size for client or server
*******************************************************************************/
sint16 ICACHE_FLASH_ATTR espconn_secure_get_size(uint8 level)
{
	if (level >= ESPCONN_MAX || level <= ESPCONN_IDLE)
		return ESPCONN_ARG;

	return max_content_len;
}

/******************************************************************************
 * FunctionName : espconn_secure_ca_enable
 * Description  : enable the certificate authenticate and set the flash sector
 * 				  as client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 *				  flash_sector -- flash sector for save certificate
 * Returns      : result true or false
*******************************************************************************/
bool ICACHE_FLASH_ATTR espconn_secure_ca_enable(uint8 level, uint32 flash_sector )
{
	if (level >= ESPCONN_MAX || level <= ESPCONN_IDLE || flash_sector <= 0)
		return false;

	if (level == ESPCONN_CLIENT){
		ssl_option.client.cert_ca_sector.sector = flash_sector;
		ssl_option.client.cert_ca_sector.flag = true;
	}

	if (level == ESPCONN_SERVER){
		ssl_option.server.cert_ca_sector.sector = flash_sector;
		ssl_option.server.cert_ca_sector.flag = true;
	}

	if (level == ESPCONN_BOTH) {
		ssl_option.client.cert_ca_sector.sector = flash_sector;
		ssl_option.server.cert_ca_sector.sector = flash_sector;
		ssl_option.client.cert_ca_sector.flag = true;
		ssl_option.server.cert_ca_sector.flag = true;
	}
	return true;
}

/******************************************************************************
 * FunctionName : espconn_secure_ca_disable
 * Description  : disable the certificate authenticate  as client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 * Returns      : result true or false
*******************************************************************************/
bool ICACHE_FLASH_ATTR espconn_secure_ca_disable(uint8 level)
{
	if (level >= ESPCONN_MAX || level <= ESPCONN_IDLE)
		return false;

	if (level == ESPCONN_CLIENT)
		ssl_option.client.cert_ca_sector.flag = false;

	if (level == ESPCONN_SERVER)
		ssl_option.server.cert_ca_sector.flag = false;

	if (level == ESPCONN_BOTH) {
		ssl_option.client.cert_ca_sector.flag = false;
		ssl_option.server.cert_ca_sector.flag = false;
	}

	return true;
}

/******************************************************************************
 * FunctionName : espconn_secure_cert_req_enable
 * Description  : enable the client certificate authenticate and set the flash sector
 * 				  as client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 *				  flash_sector -- flash sector for save certificate
 * Returns      : result true or false
*******************************************************************************/
bool ICACHE_FLASH_ATTR espconn_secure_cert_req_enable(uint8 level, uint32 flash_sector )
{
	if (level >= ESPCONN_MAX || level <= ESPCONN_IDLE || flash_sector <= 0)
		return false;

	if (level == ESPCONN_CLIENT){
		ssl_option.client.cert_req_sector.sector = flash_sector;
		ssl_option.client.cert_req_sector.flag = true;
	}

	if (level == ESPCONN_SERVER){
		ssl_option.server.cert_req_sector.sector = flash_sector;
		ssl_option.server.cert_req_sector.flag = true;
	}

	if (level == ESPCONN_BOTH) {
		ssl_option.client.cert_req_sector.sector = flash_sector;
		ssl_option.server.cert_req_sector.sector = flash_sector;
		ssl_option.client.cert_req_sector.flag = true;
		ssl_option.server.cert_req_sector.flag = true;
	}
	return true;
}

/******************************************************************************
 * FunctionName : espconn_secure_ca_disable
 * Description  : disable the client certificate authenticate  as client or server
 * Parameters   : level -- set for client or server
 *				  1: client,2:server,3:client and server
 * Returns      : result true or false
*******************************************************************************/
bool ICACHE_FLASH_ATTR espconn_secure_cert_req_disable(uint8 level)
{
	if (level >= ESPCONN_MAX || level <= ESPCONN_IDLE)
		return false;

	if (level == ESPCONN_CLIENT)
		ssl_option.client.cert_req_sector.flag = false;

	if (level == ESPCONN_SERVER)
		ssl_option.server.cert_req_sector.flag = false;

	if (level == ESPCONN_BOTH) {
		ssl_option.client.cert_req_sector.flag = false;
		ssl_option.server.cert_req_sector.flag = false;
	}

	return true;
}

/******************************************************************************
 * FunctionName : espconn_secure_set_default_certificate
 * Description  : Load the certificates in memory depending on compile-time
 * 				  and user options.
 * Parameters   : certificate -- Load the certificate
 *				  length -- Load the certificate length
 * Returns      : result true or false
*******************************************************************************/
bool ICACHE_FLASH_ATTR espconn_secure_set_default_certificate(const uint8* certificate, uint16 length)
{
	if (certificate == NULL)
		return false;

	def_certificate = (uint8*) os_zalloc(length);
	if (def_certificate == NULL)
		return false;

	os_memcpy(def_certificate, certificate, length);
	def_certificate_len = length;
	return true;
}

/******************************************************************************
 * FunctionName : espconn_secure_set_default_private_key
 * Description  : Load the key in memory depending on compile-time
 * 				  and user options.
 * Parameters   : private_key -- Load the key
 *				  length -- Load the key length
 * Returns      : result true or false
*******************************************************************************/
bool ICACHE_FLASH_ATTR espconn_secure_set_default_private_key(const uint8* private_key, uint16 length)
{
	if (private_key == NULL)
		return false;

	def_private_key = (uint8*) os_zalloc(length);
	if (def_private_key == NULL)
		return false;

	os_memcpy(def_private_key, private_key, length);
	def_private_key_len = length;
	return true;
}

/******************************************************************************
 * FunctionName : espconn_secure_delete
 * Description  : delete the secure server host
 * Parameters   : espconn -- espconn to set for client or server
 * Returns      : result
*******************************************************************************/
sint8 ICACHE_FLASH_ATTR espconn_secure_delete(struct espconn *espconn)
{
	sint8 error = ESPCONN_OK;
	error = espconn_ssl_delete(espconn);
	if (error == ESPCONN_OK){
		os_free(def_private_key);
		def_private_key = NULL;
		os_free(def_certificate);
		def_certificate = NULL;
	}
	return error;
}
#endif
