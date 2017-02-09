##SDK Version : esp8266_nonos_sdk_v2.0.0_16_07_19
##Platform : ESP-LAUNCHER BAOARD

##Operation Steps:

1. Enter path:/home/esp8266/Share, clone ESP8266 NONOS SDK to lubuntu environment by command: 
       
	git clone https://github.com/espressif/esp8266-nonos-sample-code.git
	   
2. Change the ssid and the password of the AP you want to connect to in user_main.c:
		
	os_sprintf(stationConf.ssid, "TP-LINK-FD");
	os_sprintf(stationConf.password, "aaaaaaaa");

3. Copy the example next to bin/ folder in the SDK folder. THe SDK folder should have folders inside it like : bin, examples, third party...
	   
4. Enter example folder,select the example you want to , run ./gen_misc.sh, and follow below steps to fininsh the sample code compile:
	
		Option 1 will be automatically selected, 
		Option 2 > Enter 1. 
		Option 3 > Enter Default(Just Press enter)
		Option 4 > Enter Default(Just Press enter)
		Option 5 > Enter 5.
		Then bin files will generate in BIN_PATH folder which is bin/upgrade.
	   
5. Download bin files to ESP-LAUNCHER as below sittings.
		
		Download address of each bin files:
		blank.bin				           		  0x1FE000
		esp_init_data_default.bin			  	  0x1FC000
		boot_v1.5.bin					   		  0x00000
		user1.2048.new.5.bin			          0x01000
		
		Flash download tool settings:
		CrystalFreq: 26M
		SPI SPEED: 40MHz
		SPID MODE: QIO
		FLASH SIZE: 16Mbit-C11
			
##FOR VERIFY:
	load 0x3ffe8310, len 552, room 8 
	tail 0
	chksum 0x79
	csum 0x79

	2nd boot version : 1.5
	  SPI Speed      : 40MHz
	  SPI Mode       : QIO
	  SPI Flash Size & Map: 8Mbit(512KB+512KB)
	jump to run user1 @ 1000

	rf[112] : 03
	rf[113] : 00
	rf[114] : 01

	SDK ver: 1.5.4(baaeaebb) compiled @ May 17 2016 19:23:54
	phy ver: 972, pp ver: 10.1

	SDK version:1.5.4(baaeaebb)
	data  : 0x3ffe8000 ~ 0x3ffe8810, len: 2064
	rodata: 0x3ffe8810 ~ 0x3ffe9c24, len: 5140
	bss   : 0x3ffe9c28 ~ 0x3ffeff78, len: 25424
	heap  : 0x3ffeff78 ~ 0x3fffc000, len: 49288
	sleep disable
	mode : sta(18:fe:34:ce:23:80)
	add if0
	f r0, scandone
	state: 0 -> 2 (b0)
	state: 2 -> 3 (0)
	state: 3 -> 5 (10)
	add 0
	aid 3
	cnt 

	connected with TP-LINK-FD, channel 1
	dhcp client start...
	event 0
	chg_B1:-40
	ip:192.168.20.216,mask:255.255.255.0,gw:192.168.20.1
	event 3
	heap 47152
	user_dns_found 119.9.91.208 
	espconn connect return 0 ! 
	client handshake start.
	heap size 32536 0
	mbedtls_net_send need 84
	mbedtls_net_send write 84
	mbedtls_net_recv need 5
	mbedtls_net_recv get 0
	mbedtls_net_recv need 5
	mbedtls_net_recv get 5
	mbedtls_net_recv need 81
	mbedtls_net_recv get 81
	mbedtls_net_recv need 5
	mbedtls_net_recv get 5
	mbedtls_net_recv need 2758
	mbedtls_net_recv get 1369
	mbedtls_net_recv need 1389
	mbedtls_net_recv get 0
	mbedtls_net_recv need 1389
	mbedtls_net_recv get 1389
	mbedtls_net_recv need 5
	mbedtls_net_recv get 5
	mbedtls_net_recv need 4
	mbedtls_net_recv get 4
	mbedtls_net_send need 267
	mbedtls_net_send write 267
	mbedtls_net_send need 6
	mbedtls_net_send write 6
	mbedtls_net_send need 69
	mbedtls_net_send write 69
	mbedtls_net_recv need 5
	mbedtls_net_recv get 0
	mbedtls_net_recv need 5
	mbedtls_net_recv get 5
	mbedtls_net_recv need 1
	mbedtls_net_recv get 1
	mbedtls_net_recv need 5
	mbedtls_net_recv get 5
	mbedtls_net_recv need 64
	mbedtls_net_recv get 64
	client handshake ok!
	free heap 29544
	connect succeed ! 
	mbedtls_net_send need 85
	mbedtls_net_send write 85
	send data return 0 ! 
	mbedtls_net_recv need 5
	mbedtls_net_recv get 5
	mbedtls_net_recv need 368
	mbedtls_net_recv get 368
	espconn_mebdtls.c 630 323.
	tcp recv ! length=323
	HTTP/1.1 400 Bad Request
	Server: nginx/1.8.0
	Date: Tue, 24 Jan 2017 07:23:25 GMT
	Content-Type: text/html
	Content-Length: 172
	Connection: close

	<html>
	<head><title>400 Bad Request</title></head>
	<body bgcolor="white">
	<center><h1>400 Bad Request</h1></center>
	<hr><center>nginx/1.8.0</center>
	</body>
	</html>

	mbedtls_net_recv need 5
	mbedtls_net_recv get 0
	espconn_mebdtls.c 630 -26880.
	client's data invalid protocol
	mbedtls_net_send need 53
	mbedtls_net_send write -1
	Reason:[-0x7880]
	reconnect callback, error code 8 ! 
	pm open,type:0 0
