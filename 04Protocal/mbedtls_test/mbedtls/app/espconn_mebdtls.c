/*
 * espconn_mebdtls.c
 *
 *  Created on: Apr 23, 2016
 *      Author: liuhan
 */

#if !defined(ESPCONN_MBEDTLS)

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mem.h"

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR = __FILE__;
#endif

#include "sys/socket.h"
#include "sys/espconn_mbedtls.h"

static os_event_t lwIPThreadQueue[lwIPThreadQueueLen];
static bool lwIPThreadFlag = false;
extern espconn_msg *plink_active;
static espconn_msg *plink_server = NULL;

extern unsigned int def_private_key_len;
extern unsigned char *def_private_key;
extern unsigned char *def_certificate;
extern unsigned int def_certificate_len;

//extern const size_t mbedtls_auth_certificate_len ;
//extern const char mbedtls_auth_certificate[];

static int mbedtls_setsockopt(int sock_id, int level, int optname, int optval)
{
	return setsockopt(sock_id, level, optname, (void*)&optval, sizeof(optval));
}

static int mbedtls_keep_alive(int sock_id, int onoff, int idle, int intvl, int cnt)
{
	int ret = ERR_OK;
	if (onoff == 0)
		return mbedtls_setsockopt(sock_id, SOL_SOCKET, SO_KEEPALIVE, onoff);;

	ret = mbedtls_setsockopt(sock_id, SOL_SOCKET, SO_KEEPALIVE, onoff);
	lwIP_REQUIRE_NOERROR(ret, exit);
	ret = mbedtls_setsockopt(sock_id, IPPROTO_TCP, TCP_KEEPALIVE, onoff);
	lwIP_REQUIRE_NOERROR(ret, exit);
	ret = mbedtls_setsockopt(sock_id, IPPROTO_TCP, TCP_KEEPIDLE, idle);
	lwIP_REQUIRE_NOERROR(ret, exit);
	ret = mbedtls_setsockopt(sock_id, IPPROTO_TCP, TCP_KEEPINTVL, intvl);
	lwIP_REQUIRE_NOERROR(ret, exit);
	ret = mbedtls_setsockopt(sock_id, IPPROTO_TCP, TCP_KEEPCNT, cnt);
	lwIP_REQUIRE_NOERROR(ret, exit);

exit:
	return ret;
}

static pmbedtls_msg mbedtls_msg_new(void)
{
	pmbedtls_msg msg = (pmbedtls_msg)os_zalloc( sizeof(mbedtls_msg));
	if (msg) {
		os_bzero(msg, sizeof(mbedtls_msg));
		mbedtls_net_init(&msg->listen_fd);
		mbedtls_net_init(&msg->fd);
		mbedtls_ssl_init(&msg->ssl);
		mbedtls_ssl_config_init(&msg->conf);
		mbedtls_x509_crt_init(&msg->cacert);
		mbedtls_x509_crt_init(&msg->clicert);
		mbedtls_ctr_drbg_init(&msg->ctr_drbg);
		mbedtls_pk_init(&msg->pkey);
		mbedtls_entropy_init(&msg->entropy);
	}
	return msg;
}

static void mbedtls_msg_server_step(pmbedtls_msg msg)
{
	lwIP_ASSERT(msg);

	/*to prevent memory leaks, ensure that each allocated is deleted at every handshake*/
	mbedtls_x509_crt_free(&msg->cacert);
	mbedtls_x509_crt_free(&msg->clicert);
	mbedtls_pk_free(&msg->pkey);
	mbedtls_ssl_free(&msg->ssl);
	mbedtls_ssl_config_free(&msg->conf);
	mbedtls_ctr_drbg_free(&msg->ctr_drbg);
	mbedtls_entropy_free(&msg->entropy);

	/*New connection ensure that each initial for next handshake */
	os_bzero(msg, sizeof(mbedtls_msg));
	mbedtls_net_init(&msg->fd);
	mbedtls_ssl_init(&msg->ssl);
	mbedtls_ssl_config_init(&msg->conf);
	mbedtls_x509_crt_init(&msg->cacert);
	mbedtls_x509_crt_init(&msg->clicert);
	mbedtls_ctr_drbg_init(&msg->ctr_drbg);
	mbedtls_pk_init(&msg->pkey);
	mbedtls_entropy_init(&msg->entropy);
}

static void mbedtls_msg_free(pmbedtls_msg *msg)
{
	lwIP_ASSERT(msg);
	lwIP_ASSERT(*msg);

	/*to prevent memory leaks, ensure that each allocated is deleted at every handshake*/
	mbedtls_x509_crt_free(&(*msg)->cacert);
	mbedtls_x509_crt_free(&(*msg)->clicert);
	mbedtls_pk_free(&(*msg)->pkey);
	mbedtls_ssl_free(&(*msg)->ssl);
	mbedtls_ssl_config_free(&(*msg)->conf);
	mbedtls_ctr_drbg_free(&(*msg)->ctr_drbg);
	mbedtls_entropy_free(&(*msg)->entropy);
	os_free(*msg);
	*msg = NULL;
}

static espconn_msg* mbedtls_msg_find(int sock)
{
	espconn_msg *plist = NULL;
	pmbedtls_msg msg = NULL;

	for (plist = plink_active; plist != NULL; plist = plist->pnext) {
		if(plist->pssl != NULL){
			msg = plist->pssl;
			if (msg->fd.fd == sock)
				return plist;
		}
	}

	for (plist = plink_server; plist != NULL; plist = plist->pnext){
		if(plist->pssl != NULL){
			msg = plist->pssl;
			if (msg->listen_fd.fd == sock)
				return plist;
		}
	}
	return NULL;
}

static bool mbedtls_handshake_result(const pmbedtls_msg Threadmsg)
{
	if (Threadmsg == NULL)
		return false;

	if (Threadmsg->ssl.state == MBEDTLS_SSL_HANDSHAKE_OVER) {
		int ret = 0;
		if (Threadmsg->listen_fd.fd == -1)
			ret = ssl_option.client.cert_ca_sector.flag;
		else
			ret = ssl_option.server.cert_ca_sector.flag;

		if (ret == 1){
			ret = mbedtls_ssl_get_verify_result(&Threadmsg->ssl);
			if (ret != 0) {
				char vrfy_buf[512];
				os_memset(vrfy_buf, 0, sizeof(vrfy_buf)-1);
				mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "!", ret);
				os_printf("%s\n", vrfy_buf);
				Threadmsg->verify_result = ret;
				return false;
			} else
				return true;
		} else
			return true;
	}else
		return false;
}

static void mbedtls_fail_info(espconn_msg *pinfo, int ret)
{
	pmbedtls_msg TLSmsg = NULL;
	lwIP_REQUIRE_ACTION(pinfo,exit,ret = ERR_ARG);
	TLSmsg = pinfo->pssl;
	lwIP_REQUIRE_ACTION(TLSmsg,exit,ret = ERR_ARG);

	if (TLSmsg->quiet){
		if (pinfo->preverse != NULL) {
			os_printf("server's data invalid protocol\n");
		} else {
			os_printf("client's data invalid protocol\n");
		}
		mbedtls_ssl_close_notify(&TLSmsg->ssl);
	} else{
		if (pinfo->preverse != NULL) {
			os_printf("server handshake failed!\n");
		} else {
			os_printf("client handshake failed!\n");
		}
	}

	os_printf("Reason:[-0x%2x]\n",-ret);
	/*Error code convert*/
	ret = -ret;
	if ((ret & 0xFF) != 0){
		ret = ((ret >> 8) + ret);
	} else{
		ret >>= 8;
	}
	pinfo->hs_status = -ret;

	mbedtls_net_free(&TLSmsg->fd);
exit:
	return;
}

/******************************************************************************
 * FunctionName : espconn_ssl_reconnect
 * Description  : reconnect with host
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
static void espconn_close_internal(void *arg, netconn_event event_type)
{
	espconn_msg *pssl_recon = arg;
    struct espconn *espconn = NULL;
    sint8 ssl_reerr = 0;
    sint16 hs_status = 0;
    lwIP_ASSERT(pssl_recon);

	espconn = pssl_recon->preverse;
	ssl_reerr = pssl_recon->pcommon.err;
	hs_status = pssl_recon->hs_status;
	if (espconn != NULL) {
		espconn = pssl_recon->preverse;
	} else {
		espconn = pssl_recon->pespconn;
		os_free(pssl_recon);
		pssl_recon = NULL;
	}
	
	espconn_kill_oldest_pcb();
	system_show_malloc();
	
	switch (event_type){
		case NETCONN_EVENT_ERROR:
			if (hs_status == ESPCONN_OK)
				ESPCONN_EVENT_ERROR(espconn, ssl_reerr);
			else
				ESPCONN_EVENT_ERROR(espconn, hs_status);
			break;
		case NETCONN_EVENT_CLOSE:
			if (hs_status == ESPCONN_OK)
				ESPCONN_EVENT_CLOSED(espconn);
			else
				ESPCONN_EVENT_ERROR(espconn, hs_status);
			break;
		default:
			break;
	}
}

/******************************************************************************
 * FunctionName : espconn_ssl_read_param_from_flash
 * Description  : load parameter from flash, toggle use two sector by flag value.
 * Parameters   : param--the parame point which write the flash
 * Returns      : none
*******************************************************************************/
static bool espconn_ssl_read_param_from_flash(void *param, uint16 len, int32 offset, mbedtls_auth_info *auth_info)
{
	if (param == NULL || (len + offset) > 4096) {
		return false;
	}

	uint32 FILE_PARAM_START_SEC = 0x3B;
	switch (auth_info->auth_level) {
		case ESPCONN_CLIENT:
			switch (auth_info->auth_type) {
				case ESPCONN_CERT_AUTH:
					FILE_PARAM_START_SEC = ssl_option.client.cert_ca_sector.sector;
					break;
				case ESPCONN_CERT_OWN:
				case ESPCONN_PK:
					FILE_PARAM_START_SEC = ssl_option.client.cert_req_sector.sector;
					break;
				default:
					return false;
			}
			break;
		case ESPCONN_SERVER:
			switch (auth_info->auth_type) {
				case ESPCONN_CERT_AUTH:
					FILE_PARAM_START_SEC = ssl_option.server.cert_ca_sector.sector;
					break;
				case ESPCONN_CERT_OWN:
				case ESPCONN_PK:
					FILE_PARAM_START_SEC = ssl_option.server.cert_req_sector.sector;
					break;
				default:
					return false;
			}
			break;
		default:
			return false;
			break;
	}

	spi_flash_read(FILE_PARAM_START_SEC * 4096 + offset, param, len);

	return true;
}

static bool mbedtls_msg_info_load(mbedtls_msg *msg, mbedtls_auth_info *auth_info)
{
	int ret = 0;
	int32 offerset = 0;
	uint8* load_buf = NULL;
	size_t load_len = 0;
	file_param *pfile_param = NULL;
	pfile_param = (file_param *)os_zalloc( sizeof(file_param));
	if (pfile_param==NULL)
		return false;

again:
	espconn_ssl_read_param_from_flash(&pfile_param->file_head, sizeof(file_head), offerset, auth_info);
	pfile_param->file_offerset = offerset;
	os_printf("%s %d, type[%s],length[%d]\n", __FILE__, __LINE__, pfile_param->file_head.file_name, pfile_param->file_head.file_length);
	if (pfile_param->file_head.file_length == 0xFFFF){
		os_free(pfile_param);
		return false;
	} else{
		if (auth_info->auth_type == ESPCONN_PK && os_memcmp(pfile_param->file_head.file_name, "private_key") != 0){
			offerset += sizeof(file_head) + pfile_param->file_head.file_length;
			goto again;
		}
		load_buf = (uint8_t *) os_zalloc( pfile_param->file_head.file_length);
		if (load_buf == NULL){
			os_free(pfile_param);
			return false;
		}
		offerset = sizeof(file_head) + pfile_param->file_offerset;
		espconn_ssl_read_param_from_flash(load_buf,	pfile_param->file_head.file_length, offerset, auth_info);
	}

	load_len = pfile_param->file_head.file_length;
	switch (auth_info->auth_type){
	case ESPCONN_CERT_AUTH:
		/*Optional is not optimal for security*/
		ret = mbedtls_x509_crt_parse(&msg->cacert, (const uint8*) load_buf,load_len);
		mbedtls_ssl_conf_authmode(&msg->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
		mbedtls_ssl_conf_ca_chain(&msg->conf, &msg->cacert, NULL);
		break;
	case ESPCONN_CERT_OWN:
		ret = mbedtls_x509_crt_parse(&msg->clicert, (const uint8*) load_buf,load_len);
		break;
	case ESPCONN_PK:
		ret = mbedtls_pk_parse_key(&msg->pkey, (const uint8*) load_buf,load_len, NULL, 0);
		ret = mbedtls_ssl_conf_own_cert(&msg->conf, &msg->clicert, &msg->pkey);
		break;
	}
	os_free(load_buf);
	os_free(pfile_param);
	if (ret < 0){
		return false;
	}else{
		return true;
	}
}

static bool mbedtls_msg_config(mbedtls_msg *msg)
{
	const char *pers = NULL;
	uint8 auth_type = 0;
	bool load_flag = false;
	int ret = ESPCONN_OK;
	mbedtls_auth_info auth_info;

	/*end_point mode*/
	if (msg->listen_fd.fd == -1){
		pers = "client";
		auth_type = MBEDTLS_SSL_IS_CLIENT;
	} else {
		pers = "server";
		auth_type = MBEDTLS_SSL_IS_SERVER;
	}

	/*Initialize the RNG and the session data*/
	ret = mbedtls_ctr_drbg_seed(&msg->ctr_drbg, mbedtls_entropy_func, &msg->entropy, (const unsigned char*) pers, os_strlen(pers));
	lwIP_REQUIRE_NOERROR(ret, exit);

	if (auth_type == MBEDTLS_SSL_IS_SERVER){
		/*Load the certificate and private RSA key*/
		ret = mbedtls_x509_crt_parse(&msg->clicert, (const unsigned char *)def_certificate, def_certificate_len);
		lwIP_REQUIRE_NOERROR(ret, exit);
		ret = mbedtls_pk_parse_key(&msg->pkey, (const unsigned char *)def_private_key, def_private_key_len, NULL, 0);
		lwIP_REQUIRE_NOERROR(ret, exit);
		ret = mbedtls_ssl_conf_own_cert(&msg->conf, &msg->clicert, &msg->pkey);
		lwIP_REQUIRE_NOERROR(ret, exit);

		/*Load the trusted CA*/
		if (ssl_option.server.cert_ca_sector.flag) {
			auth_info.auth_level = ESPCONN_SERVER;
			auth_info.auth_type = ESPCONN_CERT_AUTH;
			load_flag = mbedtls_msg_info_load(msg, &auth_info);
			lwIP_REQUIRE_ACTION(load_flag, exit, ret = ESPCONN_MEM);
		}
	} else{
		/*Load the certificate and private RSA key*/
		if (ssl_option.client.cert_req_sector.flag) {
			auth_info.auth_level = ESPCONN_CLIENT;
			auth_info.auth_type = ESPCONN_CERT_OWN;
			load_flag = mbedtls_msg_info_load(msg, &auth_info);
			lwIP_REQUIRE_ACTION(load_flag, exit, ret = ESPCONN_MEM);
			auth_info.auth_type = ESPCONN_PK;
			load_flag = mbedtls_msg_info_load(msg, &auth_info);
			lwIP_REQUIRE_ACTION(load_flag, exit, ret = ESPCONN_MEM);
		}

		/*Load the trusted CA*/
		if(ssl_option.client.cert_ca_sector.flag){
			auth_info.auth_level = ESPCONN_CLIENT;
			auth_info.auth_type = ESPCONN_CERT_AUTH;
			//ret = mbedtls_x509_crt_parse(&msg->cacert, (const uint8*) mbedtls_auth_certificate, mbedtls_auth_certificate_len);
			load_flag = mbedtls_msg_info_load(msg, &auth_info);
			lwIP_REQUIRE_ACTION(load_flag, exit, ret = ESPCONN_MEM);
		}
	}

	/*Setup the stuff*/
	ret = mbedtls_ssl_config_defaults(&msg->conf, auth_type, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
	lwIP_REQUIRE_NOERROR(ret, exit);

	/*OPTIONAL is not optimal for security, but makes interop easier in this session*/
	if (auth_type == MBEDTLS_SSL_IS_CLIENT && ssl_option.client.cert_ca_sector.flag == false){
		mbedtls_ssl_conf_authmode(&msg->conf, MBEDTLS_SSL_VERIFY_NONE);
	}
	mbedtls_ssl_conf_authmode(&msg->conf, MBEDTLS_SSL_VERIFY_OPTIONAL);
	mbedtls_ssl_conf_ca_chain(&msg->conf, &msg->cacert, NULL);
	mbedtls_ssl_conf_rng(&msg->conf, mbedtls_ctr_drbg_random, &msg->ctr_drbg);
	mbedtls_ssl_conf_dbg(&msg->conf, NULL, NULL);

	ret = mbedtls_ssl_setup(&msg->ssl, &msg->conf);
	lwIP_REQUIRE_NOERROR(ret, exit);

	mbedtls_ssl_set_bio(&msg->ssl, &msg->fd, mbedtls_net_send, mbedtls_net_recv, NULL);

exit:
	if (ret != 0){
		return false;
	} else{
		return true;
	}
}

int __attribute__((weak)) mbedtls_parse_internal(int socket, sint8 error)
{
	int ret = ERR_OK;
	bool config_flag = false;
	espconn_msg *Threadmsg = NULL;
	pmbedtls_msg TLSmsg = NULL;
	Threadmsg = mbedtls_msg_find(socket);
	lwIP_REQUIRE_ACTION(Threadmsg, exit, ret = ERR_MEM);
	TLSmsg = Threadmsg->pssl;
	lwIP_REQUIRE_ACTION(TLSmsg, exit, ret = ERR_MEM);

	if (error == ERR_OK){
		if (TLSmsg->quiet){
			uint8 TheadBuff[2920];
			size_t ThreadLen = sizeof(TheadBuff) - 1;
			do {
//				WDT_REG_WRITE(WDT_RST_ADDRESS, 0x73);
				os_memset(TheadBuff, 0, ThreadLen);
				ret = mbedtls_ssl_read(&TLSmsg->ssl, TheadBuff, ThreadLen);
				if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE){
					ret = ESPCONN_OK;
					break;
				}

				if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY){
					break;
				}

				if (ret < 0){
					break;
				}

				ESPCONN_EVENT_RECV(Threadmsg->pespconn, TheadBuff, ret);
				ret = ESPCONN_OK;
			} while(1);
			lwIP_REQUIRE_NOERROR(ret, exit);
		} else{
			if (TLSmsg->ssl.state == MBEDTLS_SSL_HELLO_REQUEST){
				if (Threadmsg->preverse != NULL){
					struct espconn *espconn = Threadmsg->pespconn;
					struct sockaddr_in name;
					socklen_t name_len = sizeof(name);
					remot_info *pinfo = NULL;
					espconn_get_connection_info(espconn, &pinfo , ESPCONN_SSL);
					if (espconn->link_cnt == 0x01)
						return ERR_ISCONN;

					ret = mbedtls_net_accept(&TLSmsg->listen_fd, &TLSmsg->fd, NULL, 0, NULL);
					lwIP_REQUIRE_NOERROR(ret, exit);

					/*get the remote information*/
					getpeername(TLSmsg->fd.fd, (struct sockaddr*)&name, &name_len);
					espconn->proto.tcp->remote_port = htons(name.sin_port);
					os_memcpy(espconn->proto.tcp->remote_ip, &name.sin_addr.s_addr, 4);

					/*insert the node to the active connection list*/
					espconn_list_creat(&plink_active, Threadmsg);
					os_printf("server handshake start.\n");
				} else{
					os_printf("client handshake start.\n");
				}
				config_flag = mbedtls_msg_config(TLSmsg);
				if (config_flag){
//					mbedtls_keep_alive(TLSmsg->fd.fd, 1, SSL_KEEP_IDLE, SSL_KEEP_INTVL, SSL_KEEP_CNT);
					system_overclock();
					os_printf("heap size %d %d\n", system_get_free_heap_size(), TLSmsg->conf.authmode);
				} else{
					ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
					lwIP_REQUIRE_NOERROR(ret, exit);
				}
			}

			while ((ret = mbedtls_ssl_handshake(&TLSmsg->ssl)) != 0) {
//				WDT_REG_WRITE(WDT_RST_ADDRESS, 0x73);
				if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
					ret = ESPCONN_OK;
					break;
				} else{
					break;
				}
			}
			lwIP_REQUIRE_NOERROR(ret, exit);
			/**/
			TLSmsg->quiet = mbedtls_handshake_result(TLSmsg);
			if (TLSmsg->quiet){
				if (Threadmsg->preverse != NULL) {
					os_printf("server handshake ok!\n");
				} else {
					os_printf("client handshake ok!\n");
				}
//				mbedtls_keep_alive(TLSmsg->fd.fd, 0, SSL_KEEP_IDLE, SSL_KEEP_INTVL, SSL_KEEP_CNT);
				system_restoreclock();
				os_printf("free heap %d\n", system_get_free_heap_size());
				TLSmsg->SentFnFlag = true;
				ESPCONN_EVENT_CONNECTED(Threadmsg->pespconn);
			} else{
				lwIP_REQUIRE_NOERROR_ACTION(TLSmsg->verify_result, exit, ret = TLSmsg->verify_result);
			}
		}
	} else if (error < 0){
		Threadmsg->pcommon.err = error;
		ets_post(lwIPThreadPrio, NETCONN_EVENT_ERROR, (uint32)Threadmsg);
	} else {
		ret = MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY;
		lwIP_REQUIRE_NOERROR(ret, exit);
	}

exit:
	if (ret != ESPCONN_OK){
		mbedtls_fail_info(Threadmsg, ret);		
		ets_post(lwIPThreadPrio, NETCONN_EVENT_CLOSE,(uint32)Threadmsg);
	}
	return ret;
}

/**
  * @brief  Api_Thread.
  * @param  events: contain the Api_Thread processing data
  * @retval None
  */
static void
mbedtls_thread(os_event_t *events)
{
	int ret = ESP_OK;
	espconn_msg *Threadmsg = NULL;
	pmbedtls_msg TLSmsg = NULL;
	Threadmsg = (espconn_msg *)events->par;
	lwIP_REQUIRE_ACTION(Threadmsg,exit,ret = ERR_ARG);
	TLSmsg = Threadmsg->pssl;
	lwIP_REQUIRE_ACTION(TLSmsg,exit,ret = ERR_ARG);

	/*remove the node from the active connection list*/
	espconn_list_delete(&plink_active, Threadmsg);
	if (TLSmsg->listen_fd.fd != -1){
		mbedtls_msg_server_step(TLSmsg);
	} else{
		mbedtls_msg_free(&TLSmsg);
		Threadmsg->pssl = NULL;
	}

	switch (events->sig){
		case NETCONN_EVENT_ERROR:
			espconn_close_internal(Threadmsg, NETCONN_EVENT_ERROR);
			break;
		case NETCONN_EVENT_CLOSE:
			espconn_close_internal(Threadmsg, NETCONN_EVENT_CLOSE);
			break;
		default:
			break;
	}
exit:
	return;
}

static void ICACHE_FLASH_ATTR mbedtls_threadinit(void)
{
	ets_task(mbedtls_thread, lwIPThreadPrio, lwIPThreadQueue, lwIPThreadQueueLen);
	lwIPThreadFlag = true;
}

sint8 espconn_ssl_client(struct espconn *espconn)
{
	int ret = ESPCONN_OK;
	struct ip_addr ipaddr;
	const char *server_name = NULL;
	const char *server_port = NULL;
	espconn_msg *pclient = NULL;
	pmbedtls_msg mbedTLSMsg = NULL;
	if (lwIPThreadFlag == false)
		mbedtls_threadinit();

	lwIP_REQUIRE_ACTION(espconn, exit, ret = ESPCONN_ARG);
	pclient = (espconn_msg *)os_zalloc( sizeof(espconn_msg));
	lwIP_REQUIRE_ACTION(pclient, exit, ret = ESPCONN_MEM);
	mbedTLSMsg = mbedtls_msg_new();
	lwIP_REQUIRE_ACTION(mbedTLSMsg, exit, ret = ESPCONN_MEM);
	IP4_ADDR(&ipaddr, espconn->proto.tcp->remote_ip[0],espconn->proto.tcp->remote_ip[1],
	                  espconn->proto.tcp->remote_ip[2],espconn->proto.tcp->remote_ip[3]);
	server_name = ipaddr_ntoa(&ipaddr);
	server_port = (const char *)sys_itoa(espconn->proto.tcp->remote_port);

	ESP_LOG("host%s:%s\n", server_name, server_port);

	/*start the connection*/
	ret = mbedtls_net_connect(&mbedTLSMsg->fd, server_name, server_port, MBEDTLS_NET_PROTO_TCP);
	lwIP_REQUIRE_NOERROR_ACTION(ret, exit, ret = ESPCONN_MEM);
	
	pclient->pespconn = espconn;
	pclient->pssl = mbedTLSMsg;
	pclient->preverse = NULL;
	/*insert the node to the active connection list*/
	espconn_list_creat(&plink_active, pclient);
exit:
	if (ret != ESPCONN_OK){
		if (mbedTLSMsg != NULL)
			mbedtls_msg_free(&mbedTLSMsg);
		if (pclient != NULL)
			os_free(pclient);
	}
	return ret;
}

/******************************************************************************
 * FunctionName : espconn_ssl_server
 * Description  : as
 * Parameters   :
 * Returns      :
*******************************************************************************/
sint8  espconn_ssl_server(struct espconn *espconn)
{
	int ret = ESPCONN_OK;
	struct ip_addr ipaddr;

	const char *server_port = NULL;
	espconn_msg *pserver = NULL;
	pmbedtls_msg mbedTLSMsg = NULL;
	if (lwIPThreadFlag == false)
		mbedtls_threadinit();

	lwIP_REQUIRE_ACTION(espconn, exit, ret = ESPCONN_ARG);
	/*Creates a new server control message*/
	pserver = (espconn_msg *) os_zalloc( sizeof(espconn_msg));
	lwIP_REQUIRE_ACTION(espconn, exit, ret = ESPCONN_MEM);
	mbedTLSMsg = mbedtls_msg_new();
	lwIP_REQUIRE_ACTION(mbedTLSMsg, exit, ret = ESPCONN_MEM);

	server_port = (const char *)sys_itoa(espconn->proto.tcp->local_port);
	ESP_LOG("server_port:%s\n", server_port);

	/*start the connection*/
	ret = mbedtls_net_bind(&mbedTLSMsg->listen_fd, NULL, server_port, MBEDTLS_NET_PROTO_TCP);
	lwIP_REQUIRE_NOERROR_ACTION(ret, exit, ret = ESPCONN_MEM);
	pserver->pespconn = espconn;
	pserver->pssl = mbedTLSMsg;
	pserver->preverse = espconn;
	pserver->count_opt = MEMP_NUM_TCP_PCB;
	pserver->pcommon.timeout = 0x0a;
	espconn->state = ESPCONN_LISTEN;
	plink_server = pserver;
exit:
	if (ret != ESPCONN_OK) {
		if (mbedTLSMsg != NULL)
			mbedtls_msg_free(&mbedTLSMsg);
		if (pserver != NULL)
			os_free(pserver);
	}
	return ret;
}

/******************************************************************************
 * FunctionName : espconn_ssl_delete
 * Description  : delete the server: delete a listening PCB and free it
 * Parameters   : pdeletecon -- the espconn used to delete a server
 * Returns      : none
*******************************************************************************/
sint8  espconn_ssl_delete(struct espconn *pdeletecon)
{
	err_t err;
	remot_info *pinfo = NULL;
	espconn_msg *pdelete_msg = NULL;
	pmbedtls_msg mbedTLSMsg = NULL;

	if (pdeletecon == NULL)
		return ESPCONN_ARG;

	espconn_get_connection_info(pdeletecon, &pinfo, ESPCONN_SSL);
	/*make sure all the active connection have been disconnect*/
	if (pdeletecon->link_cnt != 0)
		return ESPCONN_INPROGRESS;
	else {
		pdelete_msg = plink_server;
		if (pdelete_msg != NULL && pdelete_msg->pespconn == pdeletecon) {
			mbedTLSMsg = pdelete_msg->pssl;
			mbedtls_net_free(&mbedTLSMsg->listen_fd);
			mbedtls_msg_free(&mbedTLSMsg);
			os_free(pdelete_msg);
			pdelete_msg = NULL;
			plink_server = pdelete_msg;
			if (err == ERR_OK)
				return err;
			else
				return ESPCONN_ARG;
		} else {
			return ESPCONN_ARG;
		}
	}
}

/******************************************************************************
 * FunctionName : espconn_ssl_write
 * Description  : sent data for client or server
 * Parameters   : void *arg -- client or server to send
 *                uint8* psent -- Data to send
 *                uint16 length -- Length of data to send
 * Returns      : none
*******************************************************************************/
void
espconn_ssl_sent(void *arg, uint8 *psent, uint16 length)
{
	espconn_msg *Threadmsg = arg;
	int ret = ESPCONN_OK;
	lwIP_ASSERT(Threadmsg);
	lwIP_ASSERT(psent);
	lwIP_ASSERT(length);
	pmbedtls_msg mbedTLSMsg = Threadmsg->pssl;
	lwIP_ASSERT(mbedTLSMsg);
	while ((ret = mbedtls_ssl_write(&mbedTLSMsg->ssl, psent, length)) <= 0){
		if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
			ret = 0;
			break;
		} else{
			os_printf("send ret %d\n", ret);
		}
//		WDT_REG_WRITE(WDT_RST_ADDRESS, 0x73);
	}

	ESP_LOG("%s %d %d\n", __FILE__, __LINE__, ret);
}

/******************************************************************************
 * FunctionName : espconn_ssl_disconnect
 * Description  : A new incoming connection has been disconnected.
 * Parameters   : espconn -- the espconn used to disconnect with host
 * Returns      : none
*******************************************************************************/
void espconn_ssl_disconnect(espconn_msg *Threadmsg)
{
	lwIP_ASSERT(Threadmsg);
	pmbedtls_msg mbedTLSMsg = Threadmsg->pssl;
	lwIP_ASSERT(mbedTLSMsg);
	mbedtls_net_free(&mbedTLSMsg->fd);
	ets_post(lwIPThreadPrio, NETCONN_EVENT_CLOSE, (uint32)Threadmsg);
}

void __attribute__((weak)) mbedtls_close_tcp(void *arg, err_t error)
{
	return ;
}

#endif
