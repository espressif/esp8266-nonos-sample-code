##SDK Version : esp8266_nonos_sdk_v2.0.0_16_07_19
##Requirement: 2 pieces of ESP-LAUNCHER BOARD

##Purpose:
Example of ESP-NOW between 2 ESP8266 chips.

##Procedure:
1.Parameter need to configure : MAc ID of STA and SOFT_AP

		The mac id is available in download panel 1 section of Flash Download Tool.
			STA MAC:  xx-xx-xx-xx-xx-xx    //for ESP8266 set in controller mode
			AP  MAC:  xx-xx-xx-xx-xx-xx    //for ESP8266 set in slave mode
		
		 Copy this to dat1 and dat2 array in sample code as below.
			u8 da1[6] = {0x18, 0xfe, 0x34, 0xf3, 0x71, 0x99};			//mac id of station
			u8 da2[6] = {0x1a, 0xfe, 0x34, 0xf3, 0x70, 0x25};		    //mac id of soft_ap
		
2.Enter example folder and “copy esp_now folder to sdk directory” and call ./gen_misc.sh. Then start compiling.

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
	SPID MODE: QIO
	FLASH SIZE: 8Mbit

##Result:
1.ESP8266 under station mode(controller) will do init and connect to ESP8266 under softap mode(slave).

2.ESP8266 under station mode(controller) will send "test....espnow" to slave.

##Uart0 log:

###Slave 
	
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

	This is the ESP_Now test function .......  
	mode : softAP(1a:fe:34:ed:87:e3)
	add if1
	dhcp server start:(ip:192.168.4.1,mask:255.255.255.0,gw:192.168.4.1)
	bcn 100
	esp_now init ok
	now from[a2:20:a6:00:02:c4]:test....espnow.
	now from[a2:20:a6:00:02:c4]:test....espnow.
	now from[a2:20:a6:00:02:c4]:test....espnow.
	now from[a2:20:a6:00:02:c4]:test....espnow.
	now from[a2:20:a6:00:02:c4]:test....espnow.
	now from[a2:20:a6:00:02:c4]:test....espnow.
	now from[a2:20:a6:00:02:c4]:test....espnow.

###Controller

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
	This is the ESP_Now test function ....... 

	mode : softAP(a2:20:a6:00:02:c4)
	add if1
	dhcp server start:(ip:192.168.4.1,mask:255.255.255.0,gw:192.168.4.1)
	bcn 100
	esp_now init ok
	CONTROLLER_ROLE
	This is the demo_send section .... 
	This is the demo_send section .... 
	This is the demo_send section .... 
	This is the demo_send section .... 
	This is the demo_send section .... 
	This is the demo_send section .... 
