##SDK Version: esp8266_nonos_sdk_v2.0.0_16_07_19
##Platform: ESP-LAUNCHER BOARD

##Purpose:
Test ESP8266 chip current under modem sleep mode by forcing. ESP8266 will do modem sleep and disconnect from router. You can test current.

##Procedure:
1.Please configure the ssid and the password of the AP you want to connect to in user_main.c.

	os_sprintf(stationConf.ssid, "TP-LINK-FD");
	os_sprintf(stationConf.password, "aaaaaaaa");

2.Copy the examples next to bin/ folder in the SDK folder. THe SDK folder should have folders inside it like : bin, examples, third party...Enter example folder, select the example you want to , run ./gen_misc.sh, and follow the tips and steps.

3.Select 1,1,2,0,2 during compiling step 1 to 5. Then bin files will generate in BIN_PATH folder which is bin/upgrade.

4.Download bin files to ESP-LAUNCHER as below sittings.

	Download address of each bin files
	blank.bin						0xFE000
	esp_init_data_default.bin		0xFC000
	boot_v1.6.bin					0x00000
	user1.2048.new.5.bin			0x01000
	
	Flash download tool settings.
	CrystalFreq: 26M
	SPI SPEED: 40MHz
	SPID MODE: QIO
	FLASH SIZE: 8Mbit


##Result:
1.Board would wakeup automatically when the sleeping time(parameter of wifi_fpm_do_sleep) is costed. In this code, it’s 10 second.

2.In both cases, the callback function will be called automatically. And we create a UDP server after the board wakeup and you can check the received information by serial.

3.When the board is sleeping in forced modem-mode, you can measure the current.

##Uart0 log:

	 ets Jan  8 2013,rst cause:1, boot mode:(3,2)	load 0x40100000, len 2408, room 16 	tail 8	chksum 0xe5	load 0x3ffe8000, len 776, room 0 	tail 8	chksum 0x84	load 0x3ffe8310, len 632, room 0 	tail 8	chksum 0xd8	csum 0xd8	2nd boot version : 1.6	  SPI Speed      : 40MHz	  SPI Mode       : QIO	  SPI Flash Size & Map: 8Mbit(512KB+512KB)	jump to run user1 @ 1000	rf cal sector: 251	rf[112] : 00	rf[113] : 00	rf[114] : 01	SDK ver: 2.0.0(656edbf) compiled @ Jul 19 2016 17:58:40	phy ver: 1055, pp ver: 10.2	SDK version:2.0.0(656edbf)	Modem_FORCE	force slp enable,type: 2	fpm open,type:2 0	force sleep ,wait for a while...	mode : null
	  
	 
