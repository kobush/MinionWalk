
// ========================= RC =================================

void printSettings() {

  Serial.print("Steering: ");            
  Serial.print(unSteeringMin,DEC);
  Serial.print("<");
  Serial.print(unSteeringCenter);
  Serial.print("<");
  Serial.println(unSteeringMax);
  
  Serial.print("Throttle: ");            
  Serial.print(unThrottleMin);
  Serial.print("<");
  Serial.print(unThrottleCenter);
  Serial.print("<");
  Serial.println(unThrottleMax);
}

uint16_t readChannelSetting(uint8_t nStart)
{
  uint16_t unSetting = (EEPROM.read((nStart*sizeof(uint16_t))+1)<<8);
  unSetting += EEPROM.read(nStart*sizeof(uint16_t));

  return unSetting;
}

void writeChannelSetting(uint8_t nIndex,uint16_t unSetting)
{
  EEPROM.write(nIndex*sizeof(uint16_t),lowByte(unSetting));
  EEPROM.write((nIndex*sizeof(uint16_t))+1,highByte(unSetting));
}

void readSettingsFromEEPROM()
{
  unSteeringMin = readChannelSetting(EEPROM_INDEX_STEERING_MIN);
  if(unSteeringMin < RC_MIN || unSteeringMin > RC_NEUTRAL)
  {
    unSteeringMin = RC_MIN;
  }

  unSteeringMax = readChannelSetting(EEPROM_INDEX_STEERING_MAX);
  if(unSteeringMax > RC_MAX || unSteeringMax < RC_NEUTRAL)
  {
    unSteeringMax = RC_MAX;
  }
  
  unSteeringCenter = readChannelSetting(EEPROM_INDEX_STEERING_CENTER);
  if(unSteeringCenter < unSteeringMin || unSteeringCenter > unSteeringMax)
  {
    unSteeringCenter = RC_NEUTRAL;
  }

  unThrottleMin = readChannelSetting(EEPROM_INDEX_THROTTLE_MIN);
  if(unThrottleMin < RC_MIN || unThrottleMin > RC_NEUTRAL)
  {
    unThrottleMin = RC_MIN;
  }

  unThrottleMax = readChannelSetting(EEPROM_INDEX_THROTTLE_MAX);
  if(unThrottleMax > RC_MAX || unThrottleMax < RC_NEUTRAL)
  {
    unThrottleMax = RC_MAX;
  }
  
  unThrottleCenter = readChannelSetting(EEPROM_INDEX_THROTTLE_CENTER);
  if(unThrottleCenter < unThrottleMin || unThrottleCenter > unThrottleMax)
  {
    unThrottleCenter = RC_NEUTRAL;
  }

  printSettings();
}

void writeSettingsToEEPROM()
{
  writeChannelSetting(EEPROM_INDEX_STEERING_MIN,unSteeringMin);
  writeChannelSetting(EEPROM_INDEX_STEERING_MAX,unSteeringMax);
  writeChannelSetting(EEPROM_INDEX_STEERING_CENTER,unSteeringCenter);
  writeChannelSetting(EEPROM_INDEX_THROTTLE_MIN,unThrottleMin);
  writeChannelSetting(EEPROM_INDEX_THROTTLE_MAX,unThrottleMax);
  writeChannelSetting(EEPROM_INDEX_THROTTLE_CENTER,unThrottleCenter);

  printSettings();
}

// simple interrupt service routine
void calcThrottle()
{
  // if the pin is high, its a rising edge of the signal pulse, so lets record its value
  if(digitalRead(THROTTLE_IN_PIN) == HIGH)
  { 
    ulThrottleStart = micros();
  }
  else
  {
    // else it must be a falling edge, so lets get the time and subtract the time of the rising edge
    // this gives use the time between the rising and falling edges i.e. the pulse duration.
    unThrottleInShared = (uint16_t)(micros() - ulThrottleStart);
    // use set the throttle flag to indicate that a new throttle signal has been received
    bUpdateFlagsShared |= THROTTLE_FLAG;
  }
}

void calcSteering()
{
  if(digitalRead(STEERING_IN_PIN) == HIGH)
  { 
    ulSteeringStart = micros();
  }
  else
  {
    unSteeringInShared = (uint16_t)(micros() - ulSteeringStart);
    bUpdateFlagsShared |= STEERING_FLAG;
  }
}

void initRC() {
  readSettingsFromEEPROM();

  // tell the Arduino we want the function calcInput to be called whenever INT0 (digital pin 2) changes from HIGH to LOW or LOW to HIGH
  // catching these changes will allow us to calculate how long the input pulse is
  attachInterrupt(THROTTLE_IN, calcThrottle, CHANGE);
  attachInterrupt(STEERING_IN, calcSteering, CHANGE);
}
