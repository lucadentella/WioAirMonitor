void sendDataToAIO() {

  // get current location
  double lat = 0;
  double lon = 0;
  int ele = 0;
  if(gps.location.isValid()) {
    lat = gps.location.lat();
    lon = gps.location.lng();
  }
  if(gps.altitude.isValid()) ele = (int)gps.altitude.meters();
      
  sprintf(aio_buf, "{\"feeds\": {\"pm1\": %d,\"pm25\": %d,\"pm10\": %d},\"location\": {\"lat\": %f,\"lon\": %f,\"ele\": %d}}", 
    pm1value, pm25value, pm10value, lat, lon, ele);
  
  client.publish(adafruitTopicGroup, aio_buf);
}
