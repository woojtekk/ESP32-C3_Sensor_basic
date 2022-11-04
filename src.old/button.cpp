// void Button_Change_State()
// {

//   uint32_t start = millis();
//   uint32_t diff = -1000;
//   while( (digitalRead(PIN_BUTTON) == LOW) && (millis()-start <= 10000)){; }

//   diff = (millis()-start );
//   if (diff >= DEBOUNCE_SENSOR_MS)
//   {

//     if(diff >  2000) {
//       press_long  = true;
//       press_short = false;
//     }else {
//       press_long  = false;
//       press_short = true;
//     }
//     // else{
//     //   press_long  = false;
//     //   press_short = false;
//     // }
//   g_sensorValueChanged = true;
//   }
// }