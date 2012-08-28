#ifndef RC_H
#define RC_H

#include <Arduino.h>

#define RC_NEUTRAL 1500
#define RC_MAX 2000
#define RC_MIN 1000
#define RC_DEADBAND 40

uint16_t unSteeringMin = RC_MIN;
uint16_t unSteeringMax = RC_MAX;
uint16_t unSteeringCenter = RC_NEUTRAL;

uint16_t unThrottleMin = RC_MIN;
uint16_t unThrottleMax = RC_MAX;
uint16_t unThrottleCenter = RC_NEUTRAL;

#define THROTTLE_IN 0
#define THROTTLE_IN_PIN 2

#define STEERING_IN 1
#define STEERING_IN_PIN 3

// These bit flags are set in bUpdateFlagsShared to indicate which
// channels have new signals
#define THROTTLE_FLAG 1
#define STEERING_FLAG 2

// holds the update flags defined above
volatile uint8_t bUpdateFlagsShared;

volatile uint16_t unThrottleInShared;
volatile uint16_t unSteeringInShared;

uint32_t ulThrottleStart;
uint32_t ulSteeringStart;

// Index into the EEPROM Storage assuming a 0 based array of uint16_t
// Data to be stored low byte, high byte
#define EEPROM_INDEX_STEERING_MIN 0
#define EEPROM_INDEX_STEERING_MAX 1
#define EEPROM_INDEX_STEERING_CENTER 2
#define EEPROM_INDEX_THROTTLE_MIN 3
#define EEPROM_INDEX_THROTTLE_MAX 4
#define EEPROM_INDEX_THROTTLE_CENTER 5


void initRC();
void readSettingsFromEEPROM();

#endif
