// Wio Terminal Air Monitor
// Luca Dentella, 2022

#include "airmonitor.h"

void setup() {

  // init serial communication
  Serial.begin(115200);
  while(!Serial);
  Serial.println("AirMonitor v1.0");
  Serial.println();

  // init air sensor
  if(airSensor.init()) {
    Serial.println("- HM3301 air sensor init failed :(");
    while (1);
  }
  Serial.println("- HM3301 air sensor init done");

  // init LvGL
  lvglInit();
  Serial.println("- TFT and LvGL init done");
  #if LV_USE_LOG != 0
  lv_log_register_print_cb(my_print);
  Serial.println("- LvGL debug log active");
  #endif

  // init GPS
  Serial3.begin(9600);
  pinPeripheral(D0, PIO_SERCOM_ALT);
  pinPeripheral(D1, PIO_SERCOM_ALT);

  // init WiFi
  WiFi.begin(wifiSSID, wifiPassword);
  Serial.println("- WiFi init done");

  // init MQTT
  client.setServer(server, 1883);
  if(client.connect(ID, IO_USERNAME, IO_KEY)) 
    Serial.println("- Connected to Adafruit IO"); 
  else Serial.println("- Not connected to Adafruit IO");
  
  // init 5-way switch
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);
  Serial.println("- 5-way switch init done");

  drawScreen();

  Serial.println();
  Serial.println("Ready!");
}

void loop() {

  // LVGL tick and task handlers
  lv_tick_handler();
  lv_task_handler();

  // MQTT client loop
  client.loop();

  // Feed GPS with data
  while (Serial3.available() > 0) gps.encode(Serial3.read());
    
  // check 5-way switch
  checkSwitch();

  // update air sensor values
  if(millis() > lastAirSensorUpdated + AIRSENSOR_UPD_PERIOD) {

    if(airSensorRead()) {

      // get current values
      pm1value = airSensorData[1];
      pm25value = airSensorData[2];
      pm10value = airSensorData[3];

      // update values on screen
      updateValue();

    } else Serial.println("Unable to get data from the air sensor");
    lastAirSensorUpdated = millis();
  }

  // send data to Adafruit IO
  if(millis() > lastAioUpdated + AIO_UPD_PERIOD) {

    if(periodicSendActive) {
      sendDataToAIO();
      aioUpdateCount++;
      sprintf(sprintf_buf, "Sent data to Adafruit, %d", aioUpdateCount);
      Serial.println(sprintf_buf);
      lv_label_set_text(statusLabel, sprintf_buf);
    }
    lastAioUpdated = millis();
  }
  
}

// SERCOM Handlers
void SERCOM4_0_Handler() {
  Serial3.IrqHandler();
}
void SERCOM4_1_Handler() {
  Serial3.IrqHandler();
}
void SERCOM4_2_Handler() {
  Serial3.IrqHandler();
}
void SERCOM4_3_Handler() {
  Serial3.IrqHandler();
}
