// --- LIBRARIES ---
// PM2.5 Sensor (HM3301)
// https://github.com/Seeed-Studio/Seeed_PM2_5_sensor_HM3301
#include "Seeed_HM330X.h"

// Seeed Arduino LvGL
// https://github.com/Seeed-Studio/Seeed_Arduino_LvGL
#include <lvgl.h>
#include <TFT_eSPI.h>
//#define LV_USE_LOG 1

// Seeed Libraries for WiFi
// https://wiki.seeedstudio.com/Wio-Terminal-Network-Overview/
#include <rpcWiFi.h>

// PubSubCLient Library for MQTT
// https://pubsubclient.knolleary.net/
#include <PubSubClient.h>

// TinyGPS++ for GPS
// https://github.com/mikalhart/TinyGPSPlus
#include <wiring_private.h>
#include <TinyGPSPlus.h>


// --- DEFINES ---

// air sensor
#define AIRSENSOR_UPD_PERIOD  10000

// Adafruit IoT
#define AIO_UPD_PERIOD  60000
const char* IO_USERNAME  = "MyAIOusername";
const char* IO_KEY       = "MyAIOkey";

// display
#define MAX_VALUE           100
#define MAX_ARC             220
#define WARN_THRESHOLD      30
#define CRITICAL_THRESHOLD  50

// screens
#define SCREEN_SUMMARY  0
#define SCREEN_PM1      1
#define SCREEN_PM25     2
#define SCREEN_PM10     3
#define SCREEN_LOGGING  4
#define LAST_SCREEN     4


// --- GLOBAL VARIABLES ---

// HM3301 sensor
HM330X airSensor;
uint8_t buf[30];
uint16_t airSensorData[7];
uint16_t pm1value = 0;
uint16_t pm25value = 0;
uint16_t pm10value = 0;

// Wifi and MQTT
const char *wifiSSID = "MyWifiSSID";
const char *wifiPassword = "MyWifiPWD";
const char *server = "io.adafruit.com";
const char *ID = "Wio-Terminal-Client";
const char *adafruitTopicGroup = "MyAIOusername/groups/airmonitor";
WiFiClient wifiClient;
PubSubClient client(wifiClient);
bool periodicSendActive = false;
long lastAioUpdated = 0;
long aioUpdateCount = 0;


// GPS
static Uart Serial3(&sercom4, D1, D0, SERCOM_RX_PAD_1, UART_TX_PAD_0);
TinyGPSPlus gps;

// ai sensor refresh periods
long lastAirSensorUpdated = 0;

// active screen
int activeScreen = SCREEN_SUMMARY;

// buttons
bool upPressed = false;
bool downPressed = false;
bool rightPressed = false;
bool leftPressed = false;
bool clickPressed = false;

// TFT
TFT_eSPI tft = TFT_eSPI();

// LvGL buffers
static lv_disp_buf_t disp_buf;
static lv_color_t color_buf[LV_HOR_RES_MAX * 10];

// LvGL colors and styles
lv_color_t bgColor, titleColor, bgArcColor, sumLabelColor;
lv_style_t bgStyle, titleStyle, summaryLabelStyle, statusLabelStyle;
lv_style_t valueOkStyle, valueWarnStyle, valueCriticalStyle; 
lv_style_t arcBgStyle, arcFgOkStyle, arcFgWarnStyle, arcFgCriticalStyle;

// LvGL objects
lv_obj_t *pmArc, *pmLabel;
lv_obj_t *sumPm1Label, *sumPm25Label, *sumPm10Label;
lv_group_t *loggingGroup;
lv_obj_t *periodicSendButton, *singleSendButton;
lv_obj_t *periodicSendLabel, *singleSendLabel, *statusLabel;

// buffers
char sprintf_buf[100];
char aio_buf[100];
