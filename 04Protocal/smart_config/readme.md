##SDK Version : esp8266_nonos_sdk_v2.0.0_16_07_19
##Requirement: ESP-LAUNCHER BOARD

##Purpose:
Example of ESP-touch or Airkiss.

##Procedure:
1.Copy the examples next to bin/ folder in the SDK folder. THe SDK folder should have folders inside it like : bin, examples, third party...Enter example folder, select the example you want to , run ./gen_misc.sh, and follow the tips and steps.

2.Select 1,1,2,0,2 during compiling step 1 to 5. Then bin files will generate in BIN_PATH folder which is bin/upgrade.


3.Download bin files to ESP-LAUNCHER as below sittings.Download address of each bin files

	blank.bin						0xFE000
	esp_init_data_default.bin		0xFC000
	boot_v1.6.bin					0x00000
	user1.2048.new.2.bin			0x01000
	
	Flash download tool settings.
	CrystalFreq: 26M
	SPI SPEED: 40MHz
	SPID MODE: QIO
	FLASH SIZE: 8Mbit

##Result:
1.Boot up.

2.Connect your phone to router.
#####For ESP-touch,
3.Open ESP-touch APP and configure your device to router.
#####For Airkiss,
4.You can scan the two-dimension code in your wechat.(You can find the two-dimension code in "model two-dimension code.rar.).You can make the board connect to a router in the wechar after scanning the two-dimension code.

##Uart0 log:

	Take Airkiss as an example.
	
	ets Jan  8 2013,rst cause:1, boot mode:(3,2)	load 0x40100000, len 2408, room 16 	tail 8	chksum 0xe5	load 0x3ffe8000, len 776, room 0 	tail 8	chksum 0x84	load 0x3ffe8310, len 632, room 0 	tail 8	chksum 0xd8	csum 0xd8	2nd boot version : 1.6	  SPI Speed      : 40MHz	  SPI Mode       : QIO	  SPI Flash Size & Map: 8Mbit(512KB+512KB)	jump to run user1 @ 1000	rf cal sector: 251	rf[112] : 00	rf[113] : 00	rf[114] : 01	SDK ver: 2.0.0(656edbf) compiled @ Jul 19 2016 17:58:40	phy ver: 1055, pp ver: 10.2	SDK version:2.0.0(656edbf)	SC version: V2.5.4	mode : sta(18:fe:34:ed:87:e3)	add if0	scandone	scandone	SC_STATUS_FIND_CHANNEL	TYPE: AIRKISS	T|PHONE MAC: 38 a4 ed bd f2 24	T|AP MAC   : f0 b4 29 2c 89 05	SC_STATUS_GETTING_SSID_PSWD	SC_TYPE:SC_TYPE_AIRKISS	T|pswd : espressif0-1	T|ssid : Smart1ap	SC_STATUS_LINK	scandone	state: 0 -> 2 (b0)	state: 2 -> 3 (0)	state: 3 -> 5 (10)	add 0	aid 2	cnt 	connected with Smart1ap, channel 11	dhcp client start...	ip:192.168.31.157,mask:255.255.255.0,gw:192.168.31.1
	SC_STATUS_LINK_OVER	remote ip: 192.168.31.190 	remote port: 12476 	?S{"deviceInfo":{"deviceType":"gh_9e2cff3dfa51","deviceId":"122475"}}	Finish send notify!	Pack is not ssdq req!0	Pack is not ssdq req!0	remote ip: 192.168.31.190 	remote port: 12476 