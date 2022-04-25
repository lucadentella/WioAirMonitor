# Firmware configuration :wrench:

Open airmonitor.h tab in Arduino IDE

**1. Thresholds**

    #define WARN_THRESHOLD      30
    #define CRITICAL_THRESHOLD  50
	#define MAX_VALUE           100

Warning (= value in yellow) and Critical (= value in red) thresholds for particle concentration and maximum value (= full scale)

**2. Update periods**

    #define AIRSENSOR_UPD_PERIOD  10000
    #define AIO_UPD_PERIOD  60000

Both in milliseconds, define the update period for the dust sensor and for cloud logging (by default, data is sent to Adafruit every 60 seconds)

**3. Adafruit IO account and topic group**

    const char* IO_USERNAME  = "MyAIOusername";
	const char* IO_KEY       = "MyAIOkey";
	const char *adafruitTopicGroup = "MyAIOusername/groups/airmonitor";
	
Update the variables with your Adafruit IO accont and key, as [in the documentation](https://io.adafruit.com/api/docs/#authentication)
Make sure to create the topic group ad explained in [Dashboard configuration](https://github.com/lucadentella/WioAirMonitor/tree/main/documentation/dashboard-config.md)

**4. Wifi network** 

	const char *wifiSSID = "MyWifiSSID";
	const char *wifiPassword = "MyWifiPWD";
	
Update the variables with your Wifi settings
