#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"

#include "conn.h"

#define REMOTE_TCP_PORT 443
#define SSL_CLIENT_CERT_ADDR 0x70
#define MBEDTLS_FRAGMENT_SIZE 8192
#define SSL_TARGET_HOST_NAME "iot.espressif.com"

LOCAL char sendbuff[] = HTTP_REQUEST;

LOCAL ip_addr_t tcp_server_ip;
LOCAL os_timer_t user_dns_timer; // user_http_timer;
LOCAL struct _esp_tcp user_tcp;
LOCAL struct espconn user_tcp_conn;

LOCAL void ICACHE_FLASH_ATTR
user_conn_server(struct espconn *pespconn)
{
    pespconn->proto.tcp->local_port = espconn_port();
    ESP_DBG("espconn connect return %d ! \r\n", espconn_secure_connect(pespconn));
}

LOCAL void ICACHE_FLASH_ATTR
user_send_data(struct espconn *pespconn)
{   
	  ESP_DBG("send data return %d ! \r\n", espconn_secure_send(pespconn, sendbuff, os_strlen(sendbuff)));
}


LOCAL void ICACHE_FLASH_ATTR
user_tcp_recv_cb(void *arg, char *pusrdata, unsigned short length)
{
    //received some data from tcp connection
    ESP_DBG("tcp recv ! length=%d\r\n", length);
    ESP_DBG("%s\n", pusrdata);
}

LOCAL void ICACHE_FLASH_ATTR
user_tcp_sent_cb(void *arg)
{
    //data sent successfully
    ESP_DBG("tcp sent succeed ! \r\n");
}

LOCAL void ICACHE_FLASH_ATTR
user_tcp_discon_cb(void *arg)
{
    //tcp disconnect successfully
    ESP_DBG("tcp disconnect succeed %d! \r\n", system_get_free_heap_size());
}

LOCAL void ICACHE_FLASH_ATTR
user_tcp_recon_cb(void *arg, sint8 err)
{
    //error occured , tcp connection broke. user can try to reconnect here.
    ESP_DBG("reconnect callback, error code %d ! \r\n", err);
}

LOCAL void ICACHE_FLASH_ATTR
user_tcp_connect_cb(void *arg)
{
    struct espconn *pespconn = arg;
    
    ESP_DBG("connect succeed ! \r\n");
    espconn_regist_recvcb(pespconn, user_tcp_recv_cb);
    espconn_regist_sentcb(pespconn, user_tcp_sent_cb);

    user_send_data(pespconn);
}

LOCAL void ICACHE_FLASH_ATTR
user_dns_found(const char *name, ip_addr_t *ipaddr, void *arg)
{
    struct espconn *pespconn = arg;

    if (tcp_server_ip.addr)
        return ;
    
    if (ipaddr && ipaddr->addr)   //dns got ip succeed
    {
        ESP_DBG("user_dns_found %d.%d.%d.%d \r\n", *((uint8 *)&ipaddr->addr), *((uint8 *)&ipaddr->addr + 1), *((uint8 *)&ipaddr->addr + 2), *((uint8 *)&ipaddr->addr + 3));

        tcp_server_ip.addr = ipaddr->addr;
        os_memcpy(pespconn->proto.tcp->remote_ip, &ipaddr->addr, 4);
        os_timer_disarm(&user_dns_timer);
        user_conn_server(pespconn);
    }
    else
    {
        ESP_DBG("user_dns_found NULL \r\n");
    }
}

LOCAL void ICACHE_FLASH_ATTR
user_dns_check_cb(void *arg)
{
    struct espconn *pespconn = arg;
    if (tcp_server_ip.addr)
    {
        os_timer_disarm(&user_dns_timer);
    }
    else
    {
        espconn_gethostbyname(pespconn, SSL_TARGET_HOST_NAME, &tcp_server_ip, user_dns_found); //call DNS function
    }
}


void ICACHE_FLASH_ATTR
user_conn_init(void)
{
    user_tcp.remote_port = REMOTE_TCP_PORT;
    user_tcp_conn.proto.tcp = &user_tcp;
    user_tcp_conn.type = ESPCONN_TCP;
    user_tcp_conn.state = ESPCONN_NONE;

  	espconn_secure_set_size(ESPCONN_CLIENT, MBEDTLS_FRAGMENT_SIZE);
    //espconn_secure_ca_enable(ESPCONN_CLIENT, SSL_CLIENT_CERT_ADDR);
    //espconn_secure_cert_req_enable(ESPCONN_CLIENT, SSL_CLIENT_CERT_ADDR);
    espconn_regist_connectcb(&user_tcp_conn, user_tcp_connect_cb); // register connect callback
    espconn_regist_disconcb(&user_tcp_conn, user_tcp_discon_cb);
    espconn_regist_reconcb(&user_tcp_conn, user_tcp_recon_cb); // register reconnect callback as error handler

    os_timer_disarm(&user_dns_timer);
    os_timer_setfn(&user_dns_timer, user_dns_check_cb, &user_tcp_conn);
    os_timer_arm(&user_dns_timer, 1, 1);
}

