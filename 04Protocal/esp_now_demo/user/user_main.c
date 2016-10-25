#include "C_types.h"
#include "osapi.h"
#include "driver/uart.h"
#include "gpio.h"
#include "user_config.h"
#include "Osapi.h"
#include "Espnow.h"
#include "user_interface.h"

 
#define SLAVE	1       
#define SLAVE_ROLE 2        //slave means soft_ap + station mode
#define CONTROLLER 0   
#define CONTROLLER_ROLE  1  //controller role is in station mode
#define CHANNEL_NUM 1
char* test_str = "test....espnow.\n";   //data to be send

/*These twoo buffers store the mac id of the two ESP8266, the controller and the slave*/
u8 da1[6] = {0x18, 0xfe, 0x34, 0xf3, 0x71, 0x99};			//mac id of station
u8 da2[6] = {0x1a, 0xfe, 0x34, 0xf3, 0x70, 0x25};		    //mac id of soft_ap

os_timer_t time1;

uint32 user_rf_cal_sector_set(void)
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }
    return rf_cal_sec;
}

void ICACHE_FLASH_ATTR simple_cb(u8 *macaddr, u8 *data, u8 len)
{
	int i;
	u8 ack_buf[16];
	u8 recv_buf[200];    //recived data buffer

	/*Make received buffer zero and then copy the received data to received buffer */
	os_bzero(recv_buf,sizeof(recv_buf));
	os_memcpy(recv_buf,data,len);
	os_printf("now from["MACSTR"]:%s\n",MAC2STR(macaddr),recv_buf);
}


void demo_send_target(void *args)

{
	os_printf("This is the demo_send section .... \n");
	esp_now_send(da1, test_str, os_strlen(test_str));  //send data to mac address of soft_ap
	esp_now_send(da2, test_str, os_strlen(test_str));
}

void  loop_test()
{
	os_timer_disarm(&time1);
	os_timer_setfn(&time1,demo_send_target,NULL);
	os_timer_arm(&time1,4000,1);
}



/* use this function to configure the role of ESP8266 i.e. station or soft_ap or both */
void ICACHE_FLASH_ATTR ESP_Now_cb(void)
{
	if (esp_now_init()==0) {
		os_printf("esp_now init ok\n");
		#if CONTROLLER 
		esp_now_set_self_role(CONTROLLER_ROLE);			
		#endif
		#if SLAVE 
		esp_now_set_self_role(SLAVE_ROLE);			
		#endif
		esp_now_register_recv_cb(simple_cb);  //Callback function to print the received data.
	} else {
		os_printf("esp_now init failed\n");
	}

}

void user_init(void)
{
	os_printf("This is the ESP_Now test function .......  \n");
	#if CONTROLLER 
    loop_test();  //send data continuously after every 4 sec.
	#endif
	system_init_done_cb(ESP_Now_cb);
}

