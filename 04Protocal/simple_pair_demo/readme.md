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
#####This test need two ESP8266 board, when you #define AP, the log as follows:
		Simple Pair: STA start NEG OK
	 

   
   
   