#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"

#define FPM_SLEEP_MAX_TIME 0xFFFFFFF
#define SLEEP_MAX 1
/******************************************************************************
 * FunctionName : user_rf_cal_sector_set
 * Description  : SDK just reversed 4 sectors, used for rf init data and paramters.
 *                We add this function to force users to set rf cal sector, since
 *                we don't know which sector is free in user's application.
 *                sector map for last several sectors : ABCCC
 *                A : rf cal
 *                B : rf init data
 *                C : sdk parameters
 * Parameters   : none
 * Returns      : rf cal sector
*******************************************************************************/
uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)
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

void UdpRecvCb(void *arg, char *pdata, unsigned short len)
{
	os_printf("udp message received:\n");
	os_printf("%s\r\n", pdata);
}



/******************************************************************************
 * FunctionName : user_init
 * Description  : entry of user application, init user function here
 * Parameters   : none
 * Returns      : none
*******************************************************************************/
#include "driver/uart.h"
void user_init(void)
{
    os_printf("SDK version:%s\n", system_get_sdk_version());

	union check
	{
		int i;
		char ch;
	}c;
	c.i = 1;
	if(c.ch == 1)
		os_printf("little_endian\r\n");
	else
		os_printf("big_endian\r\n");

	struct message
	{
		unsigned char ch1;
		unsigned char ch2;
		unsigned char ch3;
		unsigned char bit0:1;
		unsigned char bit1:1;
		unsigned char bit23:2;
		unsigned char bit4:1;
		unsigned char bit5:1;
		unsigned char bit6:1;
		unsigned char bit7:1;
	}msg;
	msg.ch1=1;
	msg.ch2=0;
	msg.ch3=0;
	msg.bit0=1;
	msg.bit1=0;
	msg.bit23=0;
	//msg.bit3=0;
	msg.bit4=0;
	msg.bit5=0;
	msg.bit6=0;
	msg.bit7=0;
	os_printf("address: msg %d,  msg.ch1 %d,  msg.ch2 %d\n", &msg, &msg.ch1, &msg.ch2);
	os_printf("msg: %d\n",msg);
	struct message *p_msg=&msg;
	if((*(int*)p_msg) == 16777217)
		os_printf("msg.bit0 is the 0 bit of a BYTE\r\n");
	else
		os_printf("msg.bit0 is not the 0 bit of a BYTE\r\n");
	
	os_printf("size of uint8_t is %d bytes\n",sizeof(uint8_t));
	os_printf("size of sint8_t is %d bytes\n",sizeof(sint8_t));
	os_printf("size of int8_t is %d bytes\n",sizeof(int8_t));
	os_printf("size of uint16_t is %d bytes\n",sizeof(uint16_t));
	os_printf("size of sint16_t is %d bytes\n",sizeof(sint16_t));
	os_printf("size of int16_t is %d bytes\n",sizeof(int16_t));
	os_printf("size of uint32_t is %d bytes\n",sizeof(uint32_t));
	os_printf("size of sint32_t is %d bytes\n",sizeof(sint32_t));
	os_printf("size of int32_t is %d bytes\n",sizeof(int32_t));
	os_printf("size of sint64_t is %d bytes\n",sizeof(sint64_t));
	os_printf("size of uint64_t is %d bytes\n",sizeof(uint64_t));
	os_printf("size of u_int64_t is %d bytes\n",sizeof(u_int64_t));
	os_printf("size of real32_t is %d bytes\n",sizeof(real32_t));
	os_printf("size of real64_t is %d bytes\n",sizeof(real64_t));

	os_printf("size of uint8 is %d bytes\n",sizeof(uint8));
	os_printf("size of u8 is %d bytes\n",sizeof(u8));
	os_printf("size of sint8 is %d bytes\n",sizeof(sint8));
	os_printf("size of int8 is %d bytes\n",sizeof(int8));
	os_printf("size of s8 is %d bytes\n",sizeof(s8));
	os_printf("size of uint16 is %d bytes\n",sizeof(uint16));
	os_printf("size of u16 is %d bytes\n",sizeof(u16));
	os_printf("size of sint16 is %d bytes\n",sizeof(sint16));
	os_printf("size of s16 is %d bytes\n",sizeof(s16));
	os_printf("size of uint32 is %d bytes\n",sizeof(uint32));
	os_printf("size of u_int is %d bytes\n",sizeof(u_int));
	os_printf("size of u32 is %d bytes\n",sizeof(u32));
	os_printf("size of sint32 is %d bytes\n",sizeof(sint32));
	os_printf("size of s32 is %d bytes\n",sizeof(s32));
	os_printf("size of int is %d bytes\n",sizeof(int32));
	os_printf("size of sint64 is %d bytes\n",sizeof(sint64));
	os_printf("size of uint64 is %d bytes\n",sizeof(uint64));
	os_printf("size of u64 is %d bytes\n",sizeof(u64));
	os_printf("size of real32 is %d bytes\n",sizeof(real32));
	os_printf("size of real64 is %d bytes\n",sizeof(real64));
	
	
}
