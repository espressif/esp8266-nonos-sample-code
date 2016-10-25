##SDK Version: esp8266_nonos_sdk_v2.0.0_16_07_19
##Platform: ESP-LAUNCHER BOARD

##Purpose:
Test ESP8266 WPS function.

##Procedure:
1.Parameter need to configure : None

2.Enter example folder and “copy wps folder to sdk directory” and call ./gen_misc.sh. Then start compiling.

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
1.Boot up ESP8266.

2.Press WPS button on router.

3.ESP8266 will connect to router automatically.

##Uart0 log:

	SDK ver: 2.0.0(656edbf) compiled @ Jul 19 2016 17:58:40
	phy ver: 1055, pp ver: 10.2

	mode : sta(5c:cf:7f:0a:14:43)
	add if0
	wifi_wps_enable
	wps scan
	build public key start
	build public key finish
	wps discover [B-LINK]
	scandone
	WPS: neg start
	scandone
	state: 0 -> 2 (b0)
	state: 2 -> 3 (0)
	state: 3 -> 5 (10)
	add 0
	aid 1
	cnt 
	process pubkey start
	process pubkey finish
	WPS: key[123456789]
	wps finished
	connect_success
	wifi_wps_disable
	state: 5 -> 0 (0)
	rm 0
	scandone
	state: 0 -> 2 (b0)
	state: 2 -> 3 (0)
	state: 3 -> 5 (10)
	add 0
	aid 1
	cnt 

	connected with B-LINK, channel 2
	dhcp client start...
	ip:192.168.16.102,mask:255.255.255.0,gw:192.168.16.1
	pm open,type:2 0
