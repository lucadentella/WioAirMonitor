bool airSensorRead() {

  // read data from sensor
  if(airSensor.read_sensor_value(buf, 29) != NO_ERROR) return false;

  // verify checksum
  uint8_t checksum = 0;
  for (int i = 0; i < 28; i++) checksum += buf[i];
  if (checksum != buf[28]) return false;

  // parse data
  uint16_t value = 0;
  for (int i = 1; i < 8; i++) {
    value = (uint16_t)buf[i * 2] << 8 | buf[i * 2 + 1];
    airSensorData[i - 1] = value;
  }
  
  return true;
}

int getArcValue(int value) {

  int arcValue = value * MAX_ARC / MAX_VALUE;
  if(arcValue > MAX_ARC) arcValue = MAX_ARC;
  return arcValue;
}

lv_style_t* getArcStyle(int value) {

  if(value > CRITICAL_THRESHOLD) return &arcFgCriticalStyle;
  if(value > WARN_THRESHOLD) return &arcFgWarnStyle;
  return &arcFgOkStyle;
}

lv_style_t* getValueStyle(int value) {

  if(value > CRITICAL_THRESHOLD) return &valueCriticalStyle;
  if(value > WARN_THRESHOLD) return &valueWarnStyle;
  return &valueOkStyle;
}
