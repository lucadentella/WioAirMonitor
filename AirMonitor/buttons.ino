void checkSwitch() {

  if(digitalRead(WIO_5S_UP) == LOW) {
    if(!upPressed) {

      if(activeScreen == SCREEN_LOGGING) lv_group_focus_prev(loggingGroup);
      upPressed = true;     
    }
  } else upPressed = false;

  if(digitalRead(WIO_5S_DOWN) == LOW) {
    if(!downPressed) {

      if(activeScreen == SCREEN_LOGGING) lv_group_focus_next(loggingGroup);
      downPressed = true;     
    }
  } else downPressed = false;

  if(digitalRead(WIO_5S_PRESS) == LOW) {
    if(!clickPressed) {

      if(activeScreen == SCREEN_LOGGING) {
        lv_obj_t* clickedButton = lv_group_get_focused(loggingGroup);
        
        if(clickedButton == periodicSendButton) {
          if(periodicSendActive) {
            periodicSendActive = false;
            lv_label_set_text(periodicSendLabel, "START");
            lv_label_set_text(statusLabel, "Idle");
          } else {
            periodicSendActive = true;
            aioUpdateCount = 0;
            lv_label_set_text(periodicSendLabel, "STOP");    
            lv_label_set_text(statusLabel, "Cloud logging started");        
          }
        }
        
        else if(clickedButton == singleSendButton) {
          sendDataToAIO();
          Serial.println("Sent data to Adafruit, SINGLE");
          lv_label_set_text(statusLabel, "Sent data to Adafruit, SINGLE");
        }
      }
      clickPressed = true;     
    }
  } else clickPressed = false;

  if(digitalRead(WIO_5S_RIGHT) == LOW) {
    if(!rightPressed) {

      int newScreen = activeScreen + 1;
      if(newScreen > LAST_SCREEN) newScreen = 0;
      activeScreen = newScreen;
      drawScreen();
      rightPressed = true;     
    }
  } else rightPressed = false;

  if(digitalRead(WIO_5S_LEFT) == LOW) {
    if(!leftPressed) {
      int newScreen = activeScreen - 1;
      if(newScreen < 0) newScreen = LAST_SCREEN;
      activeScreen = newScreen;
      drawScreen();      
      leftPressed = true;
    }
  } else leftPressed = false;
  

}
