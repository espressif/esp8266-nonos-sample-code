##SDK Version: esp8266_nonos_sdk_v2.0.0_16_07_19
##Platform: ESP-LAUNCHER BOARD

##Purpose:
ESP8266 RAM usag(malloc)) test and global variable and local variable test.

##Procedure:
1.Parameter need to configure : None

2.Enter example folder and “copy system_feature folder to sdk directory” and call ./gen_misc.sh. Then start compiling.

3.Select 1,1,enter,enter,2 during compiling step 1 to 5. Then bin files will generate in BIN_PATH folder which is bin/upgrade.

4.Download bin files to ESP-LAUNCHER as below sittings.

	Download address of each bin files
	blank.bin								0xFE000
	esp_init_data_default.bin				0xFC000
	boot_v1.6.bin							0x00000
	user1.2048.new.2.bin					0x01000
	
	Flash download tool settings.
	CrystalFreq: 26M
	SPI SPEED: 40MHz
	SPID MODE: DIO
	FLASH SIZE: 8Mbit
	
##Result:
See uart0 log and source code.

##Uart0 log:

	  ets Jan  8 2013,rst cause:1, boot mode:(3,2)

	load 0x40100000, len 2408, room 16 
	tail 8
	chksum 0xe5
	load 0x3ffe8000, len 776, room 0 
	tail 8
	chksum 0x84
	load 0x3ffe8310, len 632, room 0 
	tail 8
	chksum 0xd8
	csum 0xd8

	2nd boot version : 1.6
	  SPI Speed      : 40MHz
	  SPI Mode       : QIO
	  SPI Flash Size & Map: 8Mbit(512KB+512KB)
	jump to run user1 @ 1000

	rf cal sector: 251
	rf[112] : 00
	rf[113] : 00
	rf[114] : 01

	SDK ver: 2.0.0(656edbf) compiled @ Jul 19 2016 17:58:40
	phy ver: 1055, pp ver: 10.2

	SDK version:2.0.0(656edbf)

	before malloc system_get_free_heap_size=55264
	malloc system_get_free_heap_size=53208
	 free system_get_free_heap_size=55264
	little_endian
	address: msg 1073740176,  msg.ch1 1073740176,  msg.ch2 1073740177
	msg: 16777217
	msg.bit0 is the 0 bit of a BYTE
	size of uint8_t is 1 bytes
	size of sint8_t is 1 bytes
	size of int8_t is 1 bytes
	size of uint16_t is 2 bytes
	size of sint16_t is 2 bytes
	size of int16_t is 2 bytes
	size of uint32_t is 4 bytes
	size of sint32_t is 4 bytes
	size of int32_t is 4 bytes
	size of sint64_t is 8 bytes
	size of uint64_t is 8 bytes
	size of u_int64_t is 8 bytes
	size of real32_t is 4 bytes
	size of real64_t is 8 bytes
	size of uint8 is 1 bytes
	size of u8 is 1 bytes
	size of sint8 is 1 bytes
	size of int8 is 1 bytes
	size of s8 is 1 bytes
	size of uint16 is 2 bytes
	size of u16 is 2 bytes
	size of sint16 is 2 bytes
	size of s16 is 2 bytes
	size of uint32 is 4 bytes
	size of u_int is 4 bytes
	size of u32 is 4 bytes
	size of sint32 is 4 bytes
	size of s32 is 4 bytes
	size of int is 4 bytes
	size of sint64 is 8 bytes
	size of uint64 is 8 bytes
	size of u64 is 8 bytes
	size of real32 is 4 bytes
	size of real64 is 8 bytes
	mode : softAP(a2:20:a6:00:02:c4)
	add if1
	dhcp server start:(ip:192.168.4.1,mask:255.255.255.0,gw:192.168.4.1)
	bcn 100
