##SDK Version : esp8266_nonos_sdk_v2.0.0_16_07_19
##Requirement: 2 pieces of ESP-LAUNCHER BOARD

##Purpose:
Example of ESP-pair between 2 ESP8266 chips.

##Procedure:
1.Parameter need to configure
 
   While downloading the program for Station AP,  In the main.c file, make sure that "AS_STA" is defined and "AS_AP" is commented 
   While downloading the program for Soft AP,  In the main.c file, make sure that ""AS_AP" "is defined and "AS_STA" is commented
	
2.Enter example folder and "copy simple_pair folder to sdk directory" and call ./gen_misc.sh. Then start compiling.

3.Select 1,1,enter,enter,2 during compiling step 1 to 5. Then bin files will generate in BIN_PATH folder which is bin/upgrade.

4.Download bin files with below settings.

	Download address of each bin files
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
1.ESP8266 with softap mode enter Announce Mode.

2.ESP8266 with station mode send negotiation request. And send key(16 words) to softap.

##uart0 log
#####This test need two ESP8266 board, when you #define AP, the log as follows:		2nd boot version : 1.6		  SPI Speed      : 40MHz		  SPI Mode       : QIO		  SPI Flash Size & Map: 8Mbit(512KB+512KB)		jump to run user1 @ 1000		rf cal sector: 251		rf[112] : 00		rf[113] : 00		rf[114] : 01		SDK ver: 2.0.0(656edbf) compiled @ Jul 19 2016 17:58:40		phy ver: 1055, pp ver: 10.2		AP--		mode : softAP(5e:cf:7f:0a:14:43)		add if1		dhcp server start:(ip:192.168.4.1,mask:255.255.255.0,gw:192.168.4.1)		bcn 100		Simple Pair: AP Enter Announce Mode ...		Simple Pair: Recv STA Negotiate Request		Simple Pair: AP FINISH		#####when you #define STA, the log as follows:						2nd boot version : 1.6		SPI Speed      : 40MHz		SPI Mode       : QIO		SPI Flash Size & Map: 8Mbit(512KB+512KB)		jump to run user1 @ 1000		rf cal sector: 251		rf[112] : 00		rf[113] : 00		rf[114] : 01		SDK ver: 2.0.0(656edbf) compiled @ Jul 19 2016 17:58:40		phy ver: 1055, pp ver: 10.2		STA--		mode : sta(a0:20:a6:00:02:c4)		add if0		Simple Pair: STA Enter Scan Mode ...		Simple Pair: STA Scan AP ...		scandone		Simple Pair: bssid 5e:cf:7f:0a:14:43 Ready!			
		Simple Pair: STA start NEG OK		Simple Pair: STA FINISH, Ex_key ff, ee, dd, cc, bb, aa, 99, 88, 77, 66, 55, 44, 33, 22, 11, 00,		
	 

   
   
   