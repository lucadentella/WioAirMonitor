// tick period
#define LVGL_TICK_PERIOD 5

// fonts
LV_FONT_DECLARE(roboto_12);
LV_FONT_DECLARE(roboto_24);
LV_FONT_DECLARE(roboto_36);
LV_FONT_DECLARE(roboto_48);

// screen resolution
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

// display flushing callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
  
  uint16_t c;

  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, (area->x2 - area->x1 + 1), (area->y2 - area->y1 + 1));
  for (int y = area->y1; y <= area->y2; y++) {
    for (int x = area->x1; x <= area->x2; x++) {
      c = color_p->full;
      tft.writeColor(c, 1);
      color_p++;
    }
  }
  tft.endWrite(); 
  lv_disp_flush_ready(disp);
}

// tick handler
static void lv_tick_handler(void) {
  
  lv_tick_inc(LVGL_TICK_PERIOD);
}

// init TFT and LvGL
void lvglInit() {

  // TFT
  tft.begin(); 
  tft.setRotation(3);

  // Library and display buffer
  lv_init();
  lv_disp_buf_init(&disp_buf, color_buf, NULL, LV_HOR_RES_MAX * 10);

  // Display driver
  lv_disp_drv_t disp_drv;
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.buffer = &disp_buf;
  lv_disp_drv_register(&disp_drv);

  // colors
  bgColor = lv_color_make(12, 38, 52);
  titleColor = lv_color_make(160, 196, 199);
  bgArcColor = lv_color_make(72, 113, 124);
  sumLabelColor = lv_color_make(72, 123, 140);
  
  // styles
  lv_style_init(&bgStyle);
  lv_style_set_bg_color(&bgStyle, LV_STATE_DEFAULT, bgColor);
  
  lv_style_init(&titleStyle);
  lv_style_set_text_font(&titleStyle, LV_STATE_DEFAULT, &roboto_36);
  lv_style_set_text_color(&titleStyle, LV_STATE_DEFAULT, titleColor);

  lv_style_init(&summaryLabelStyle);
  lv_style_set_text_font(&summaryLabelStyle, LV_STATE_DEFAULT, &roboto_24);
  lv_style_set_text_color(&summaryLabelStyle, LV_STATE_DEFAULT, sumLabelColor);

  lv_style_init(&statusLabelStyle);
  lv_style_set_text_font(&statusLabelStyle, LV_STATE_DEFAULT, &roboto_12);
  lv_style_set_text_color(&statusLabelStyle, LV_STATE_DEFAULT, sumLabelColor);
  
  lv_style_init(&valueOkStyle);
  lv_style_set_text_font(&valueOkStyle, LV_STATE_DEFAULT, &roboto_48);
  lv_style_set_text_color(&valueOkStyle, LV_STATE_DEFAULT, LV_COLOR_GREEN);

  lv_style_init(&valueWarnStyle);
  lv_style_set_text_font(&valueWarnStyle, LV_STATE_DEFAULT, &roboto_48);
  lv_style_set_text_color(&valueWarnStyle, LV_STATE_DEFAULT, LV_COLOR_YELLOW);

  lv_style_init(&valueCriticalStyle);
  lv_style_set_text_font(&valueCriticalStyle, LV_STATE_DEFAULT, &roboto_48);
  lv_style_set_text_color(&valueCriticalStyle, LV_STATE_DEFAULT, LV_COLOR_RED);  

  lv_style_init(&arcBgStyle);
  lv_style_set_bg_color(&arcBgStyle, LV_STATE_DEFAULT, bgColor);
  lv_style_set_border_width(&arcBgStyle, LV_STATE_DEFAULT, 0);
  lv_style_set_line_width(&arcBgStyle, LV_STATE_DEFAULT, 3); 
  lv_style_set_line_color(&arcBgStyle, LV_STATE_DEFAULT, bgArcColor); 

  lv_style_init(&arcFgOkStyle);
  lv_style_set_line_width(&arcFgOkStyle, LV_STATE_DEFAULT, 10);
  lv_style_set_line_color(&arcFgOkStyle, LV_STATE_DEFAULT, LV_COLOR_GREEN);
   
  lv_style_init(&arcFgWarnStyle);
  lv_style_set_line_width(&arcFgWarnStyle, LV_STATE_DEFAULT, 10);
  lv_style_set_line_color(&arcFgWarnStyle, LV_STATE_DEFAULT, LV_COLOR_YELLOW);

  lv_style_init(&arcFgCriticalStyle);
  lv_style_set_line_width(&arcFgCriticalStyle, LV_STATE_DEFAULT, 10);
  lv_style_set_line_color(&arcFgCriticalStyle, LV_STATE_DEFAULT, LV_COLOR_RED);
}

// draw screen
void drawScreen() {

  if(activeScreen == SCREEN_PM1 || activeScreen == SCREEN_PM25 || activeScreen == SCREEN_PM10) drawSingleScreen();
  if(activeScreen == SCREEN_SUMMARY) drawSummaryScreen();
  if(activeScreen == SCREEN_LOGGING) drawLoggingScreen();
}

// draw single data screen
void drawSingleScreen() {

  // clean the screen and set background style
  lv_obj_clean(lv_scr_act()); 
  lv_obj_add_style(lv_scr_act(), LV_STATE_DEFAULT, &bgStyle);

  // title
  lv_obj_t *titleLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_add_style(titleLabel, 0, &titleStyle);
  if(activeScreen == SCREEN_PM1) lv_label_set_text(titleLabel, "PM 1.0");
  else if(activeScreen == SCREEN_PM25) lv_label_set_text(titleLabel, "PM 2.5");
  else if(activeScreen == SCREEN_PM10) lv_label_set_text(titleLabel, "PM 10");
  lv_obj_align(titleLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

  // add arc
  pmArc = lv_arc_create(lv_scr_act(), NULL);
  lv_obj_add_style(pmArc, LV_LINE_PART_MAIN, &arcBgStyle);
  lv_obj_set_size(pmArc, 200, 200);
  lv_obj_align(pmArc, lv_scr_act(), LV_ALIGN_CENTER, 0, 50);
  lv_arc_set_rotation(pmArc, 180 - ((MAX_ARC - 180) / 2));
  lv_arc_set_bg_angles(pmArc, 0, MAX_ARC);
  lv_arc_set_angles(pmArc, 0, 0);

  // add label
  pmLabel = lv_label_create(lv_scr_act(), NULL);
  
  updateValue();
}

// draw summary data screen
void drawSummaryScreen() {

  // clean the screen and set background style
  lv_obj_clean(lv_scr_act()); 
  lv_obj_add_style(lv_scr_act(), LV_STATE_DEFAULT, &bgStyle);

  // title
  lv_obj_t *titleLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_add_style(titleLabel, 0, &titleStyle);
  lv_label_set_text(titleLabel, "Summary");
  lv_obj_align(titleLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 20); 

  // static labels
  lv_obj_t *pm1Label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_long_mode(pm1Label, LV_LABEL_LONG_CROP);
  lv_obj_set_size(pm1Label, 106, 30);
  lv_obj_set_pos(pm1Label, 0, 90);
  lv_obj_add_style(pm1Label, 0, &summaryLabelStyle);
  lv_label_set_text(pm1Label, "PM 1.0");
  lv_label_set_align(pm1Label, LV_LABEL_ALIGN_CENTER);  

  lv_obj_t *pm25Label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_long_mode(pm25Label, LV_LABEL_LONG_CROP);
  lv_obj_set_size(pm25Label, 106, 30);
  lv_obj_set_pos(pm25Label, 106, 90);  
  lv_obj_add_style(pm25Label, 0, &summaryLabelStyle);
  lv_label_set_text(pm25Label, "PM 2.5");
  lv_label_set_align(pm25Label, LV_LABEL_ALIGN_CENTER);  

  lv_obj_t *pm10Label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_long_mode(pm10Label, LV_LABEL_LONG_CROP);
  lv_obj_set_size(pm10Label, 106, 30);
  lv_obj_set_pos(pm10Label, 212, 90);  
  lv_obj_add_style(pm10Label, 0, &summaryLabelStyle);
  lv_label_set_text(pm10Label, "PM 10");
  lv_label_set_align(pm10Label, LV_LABEL_ALIGN_CENTER);  

  // value labels
  sumPm1Label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_long_mode(sumPm1Label, LV_LABEL_LONG_CROP);
  lv_obj_set_size(sumPm1Label, 106, 60);
  lv_obj_set_pos(sumPm1Label, 0, 140);  
  
  sumPm25Label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_long_mode(sumPm25Label, LV_LABEL_LONG_CROP);
  lv_obj_set_size(sumPm25Label, 106, 60);
  lv_obj_set_pos(sumPm25Label, 106, 140); 

  sumPm10Label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_long_mode(sumPm10Label, LV_LABEL_LONG_CROP);
  lv_obj_set_size(sumPm10Label, 106, 60);
  lv_obj_set_pos(sumPm10Label, 212, 140); 

  updateValue();      
}

// draw logging screen
void drawLoggingScreen() {

  // clean the screen and set background style
  lv_obj_clean(lv_scr_act()); 
  lv_obj_add_style(lv_scr_act(), LV_STATE_DEFAULT, &bgStyle);

  // title
  lv_obj_t *titleLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_add_style(titleLabel, 0, &titleStyle);
  lv_label_set_text(titleLabel, "Cloud logging");
  lv_obj_align(titleLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 20); 

  // buttons
  loggingGroup = lv_group_create();
  periodicSendButton = lv_btn_create(lv_scr_act(), NULL);
  lv_group_add_obj(loggingGroup, periodicSendButton);
  periodicSendLabel = lv_label_create(periodicSendButton, NULL);
  if(periodicSendActive) lv_label_set_text(periodicSendLabel, "STOP");
  else lv_label_set_text(periodicSendLabel, "START");
  lv_btn_set_fit(periodicSendButton, LV_LAYOUT_CENTER);
  lv_obj_align(periodicSendButton, NULL, LV_ALIGN_IN_TOP_MID, 0, 90);
  singleSendButton = lv_btn_create(lv_scr_act(), NULL);
  lv_group_add_obj(loggingGroup, singleSendButton);
  singleSendLabel = lv_label_create(singleSendButton, NULL);
  lv_label_set_text(singleSendLabel, "SEND ONCE");
  lv_btn_set_fit(singleSendButton, LV_LAYOUT_CENTER);
  lv_obj_align(singleSendButton, NULL, LV_ALIGN_IN_TOP_MID, 0, 140);

  // status label
  statusLabel = lv_label_create(lv_scr_act(), NULL);
  lv_obj_add_style(statusLabel, 0, &statusLabelStyle);
  lv_label_set_text(statusLabel, "Idle");
  lv_obj_align(statusLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 200);
  lv_obj_set_auto_realign(statusLabel, true);
}

void updateValue() {

  uint16_t displayValue;
  if(activeScreen == SCREEN_PM1) displayValue = pm1value;
  else if(activeScreen == SCREEN_PM25) displayValue = pm25value;
  else if(activeScreen == SCREEN_PM10) displayValue = pm10value;

  if(activeScreen == SCREEN_PM1 || activeScreen == SCREEN_PM25 || activeScreen == SCREEN_PM10) {
    
    sprintf(sprintf_buf, "%d", displayValue);
    lv_label_set_text(pmLabel, sprintf_buf);
    lv_obj_add_style(pmLabel, LV_LABEL_PART_MAIN, getValueStyle(displayValue));
    lv_obj_align(pmLabel, pmArc, LV_ALIGN_CENTER, 0, -10);
    lv_obj_add_style(pmArc, LV_ARC_PART_INDIC, getArcStyle(displayValue));
    lv_arc_set_angles(pmArc, 0, getArcValue(displayValue)); 
  }

  if(activeScreen == SCREEN_SUMMARY) {
  
    sprintf(sprintf_buf, "%d", pm1value);
    lv_label_set_text(sumPm1Label, sprintf_buf);
    lv_obj_add_style(sumPm1Label, LV_LABEL_PART_MAIN, getValueStyle(pm1value));
    lv_label_set_align(sumPm1Label, LV_LABEL_ALIGN_CENTER);
     
    sprintf(sprintf_buf, "%d", pm25value);
    lv_label_set_text(sumPm25Label, sprintf_buf);
    lv_obj_add_style(sumPm25Label, LV_LABEL_PART_MAIN, getValueStyle(pm25value));
    lv_label_set_align(sumPm25Label, LV_LABEL_ALIGN_CENTER);  

    sprintf(sprintf_buf, "%d", pm10value);
    lv_label_set_text(sumPm10Label, sprintf_buf);
    lv_obj_add_style(sumPm10Label, LV_LABEL_PART_MAIN, getValueStyle(pm10value));
    lv_label_set_align(sumPm10Label, LV_LABEL_ALIGN_CENTER); 
  }
}

// FOR DEBUG ONLY
#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char * dsc) {

  if(level == LV_LOG_LEVEL_ERROR) Serial.print("ERROR: ");
  if(level == LV_LOG_LEVEL_WARN)  Serial.print("WARNING: ");
  if(level == LV_LOG_LEVEL_INFO)  Serial.print("INFO: ");
  if(level == LV_LOG_LEVEL_TRACE) Serial.print("TRACE: ");
  Serial.println(dsc);
}
#endif
