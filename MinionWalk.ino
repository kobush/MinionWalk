// Simple Walk 2
//this sketch is an evolution of simple walking program for the Minion
// it moves towars the directon of estabilshing some basic control

#include <EEPROM.h>
#include <Servo.h> 
#include <Button.h>

#include "RC.h"

#define BUTTON_PIN 10
#define STATUS_LED_PIN 13

#define MODE_RUN 0
#define MODE_PROGRAM 1

uint8_t gMode = MODE_RUN;
uint32_t ulProgramModeExitTime = 0; 

Button modeButton = Button(BUTTON_PIN);

Servo lfoot;  // create servo object to control Left Foot
Servo rfoot;  // create servo object to control Right Foot
Servo lleg;  // create servo object to control Left Leg
Servo rleg;  // create servo object to control Right Leg
 
int hip = 90;    // variable to store the hip servo position 
int ankle = 90;    // variable to store the ankle servo position

int hipstep = 10;  // degree of movement of the hips
int anklestep = 5;  // degree of movement of ankles

uint8_t sequence = 0;  // variable to store current state of walking sequence
uint32_t walktime = 0;  // variable to store delay untill next step sequence
uint32_t autoRestTime = 0;

#define MIN_THROTTLE 15
#define MAX_THROTTLE 50

int throttle = 0;
int steering = 20;

//Setup commands
void setup() 
{ 
  Serial.begin(9600);
  Serial.println("Minion Walk");
  
  initRC();

  pinMode(STATUS_LED_PIN, OUTPUT);
  
  lfoot.attach(8);  // attaches left foot to pin 8
  rfoot.attach(6);  // attaches right foot to pin 6
  lleg.attach(9);  // attaches left leg to pin 9
  rleg.attach(7);  // attaches right leg to pin 7
} 

//Primaty loop 
void loop() 
{ 
  // create local variables to hold a local copies of the channel inputs
  // these are declared static so that thier values will be retained 
  // between calls to loop.
  static uint16_t unThrottleIn;
  static uint16_t unSteeringIn;
  
  // local copy of update flags
  static uint8_t bUpdateFlags;

  // check shared update flags to see if any channels have a new signal
  if(bUpdateFlagsShared)
  {
    noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables
    
    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    bUpdateFlags = bUpdateFlagsShared;
    
    // in the current code, the shared values are always populated
    // so we could copy them without testing the flags
    // however in the future this could change, so lets
    // only copy when the flags tell us we can.
    if(bUpdateFlags & THROTTLE_FLAG)
    {
      unThrottleIn = unThrottleInShared;
    }
    
    if(bUpdateFlags & STEERING_FLAG)
    {
      unSteeringIn = unSteeringInShared;
    }
    
    // clear shared copy of updated flags as we have already taken the updates
    // we still have a local copy if we need to use it in bUpdateFlags
    bUpdateFlagsShared = 0;
    
    interrupts(); // enable interupts
  }
  
  if (modeButton.uniquePress()) {
    
    if (gMode == MODE_RUN)
    {
      Serial.println("Programming start");
      digitalWrite(STATUS_LED_PIN, HIGH);

      // give 10 seconds to program
      ulProgramModeExitTime = millis() + 10000;
      gMode = MODE_PROGRAM;
      throttle = 0; // stop
 
      unThrottleCenter = unThrottleIn;
      unSteeringCenter = unSteeringIn;    
      unThrottleMin = RC_NEUTRAL;
      unThrottleMax = RC_NEUTRAL;
      unSteeringMin = RC_NEUTRAL;
      unSteeringMax = RC_NEUTRAL;
    }
    else 
    {
      // abort programming
      Serial.println("aborting");
      gMode = MODE_RUN;
    }
  }
  
  if (gMode == MODE_PROGRAM) 
  {
    if(ulProgramModeExitTime < millis())
    {
      digitalWrite(STATUS_LED_PIN, LOW);
      Serial.println("Programming done");
       
      // set to 0 and exit program mode
      ulProgramModeExitTime = 0;
      gMode = MODE_RUN;
      
      writeSettingsToEEPROM();
    }
    else 
    {
      if(unThrottleIn > unThrottleMax && unThrottleIn <= RC_MAX)
        unThrottleMax = unThrottleIn;
      else if(unThrottleIn < unThrottleMin && unThrottleIn >= RC_MIN)
        unThrottleMin = unThrottleIn;
   
      if(unSteeringIn > unSteeringMax && unSteeringIn <= RC_MAX)
        unSteeringMax = unSteeringIn;
      else if(unSteeringIn < unSteeringMin && unSteeringIn >= RC_MIN)
        unSteeringMin = unSteeringIn;
    }
  }
  else if (gMode == MODE_RUN)
  {
    if (bUpdateFlags != 0)
    {
      // we got update so delay auto-rest
      autoRestTime = millis() + 100;
    }
    
    if (bUpdateFlags & THROTTLE_FLAG)
    {
        if (unThrottleIn > unThrottleCenter + RC_DEADBAND)
          throttle = map(unThrottleIn, unThrottleMin, unThrottleCenter, -MAX_THROTTLE, -MIN_THROTTLE);
        else if (unThrottleIn < unThrottleCenter - RC_DEADBAND)
          throttle = map(unThrottleIn, unThrottleCenter, unThrottleMax, MIN_THROTTLE, MAX_THROTTLE);
        else
          throttle = 0;
    }
  
    if (bUpdateFlags & STEERING_FLAG)
    {
        if (unSteeringIn > unSteeringCenter)
          steering = map(unSteeringIn, unSteeringMin, unSteeringCenter, 0,20);
        else
          steering = map(unSteeringIn, unSteeringCenter, unSteeringMax, 20,40);
    }
  }
    
   if (autoRestTime < millis())
   {
     // go to rest if no update received in last 100ms
     throttle = 0;
   }
   
  /*
    Serial.print("Throttle ");
    Serial.print(throttle, DEC);
    Serial.print(" Steering ");
    Serial.println(steering, DEC);
  */  
  
  // done with updates
  bUpdateFlags = 0;
  
  // time to walk again
  if (walktime < millis())
  {
    if(throttle == 0){
      Rest();
    }
    else{
      Walk();
    }
    
    //delay(15);  // delay for servo to get in possiton.  this needs to be replaced with some type of counter function

    // delay proportional to throttle
    walktime = millis() + MAX_THROTTLE - abs(throttle);
  }
}


//Functions

void Rest()  //function to return the walker to rest configeration from any possable state
{
  sequence = 0;  //reset the walk sequence
   
  if(ankle < 90)
  {
        rfoot.write(ankle); 
        lfoot.write(ankle);
        ankle = ankle + 5;
  }
    if ( ankle > 90){
        rfoot.write(ankle); 
        lfoot.write(ankle);
        ankle = ankle - 5;
  }
    if ( hip < (70 + steering)){
        lleg.write(hip);
        rleg.write(hip);
        hip = hip + 10;
  }
    if ( hip > (70 + steering)){
        lleg.write(hip);
        rleg.write(hip);
        hip = hip - 10;
  }
  
}
void Walk() {  //Function to allow the robot to walk forwards and backwards
  
   if(sequence == 0)  // Rotatates bolth ankles to lift right foot 
  { 

      if(ankle <= 115){      // checks to see if ankle is at 135 degrees
        rfoot.write(ankle); 
        lfoot.write(ankle);
        ankle = ankle + anklestep;
        
        
        //lleg.write(hip + steering);
        //rleg.write(hip + steering);
        
     if(steering < 28)
      lleg.write(hip + steering);

      if(steering > 28)
       rleg.write(hip + steering); 

      
      }
      else{
        if(throttle >= 0){
          sequence = 1;
        }
        else{
          sequence = 3;
        }
      }                      
  } 
  
    if(sequence == 1)  // Rotatates bolth hips to move right foot  
  {
      if(hip >= 45){  // was 25.  checks to see if hips are at 45 if not they are moved by the amount dictated in hipstep 
        lleg.write(hip);
        rleg.write(hip);
        hip = hip - hipstep;
      }
      else{  //once we reach 45, the sequence number is updated to 2
         if(throttle >= 0){
          sequence = 2;
        }
        else{
          sequence = 0; 
      }     
  }}
  
    if(sequence == 2)  // Rotatates bolth hips to lift left foot  
  { 

    if(ankle >= 75){    
      lfoot.write(ankle);
      rfoot.write(ankle); 
      ankle = ankle - anklestep;
      
      //rleg.write(hip + steering);
      //lleg.write(hip + steering);
      
      
      if(steering < 28)
      lleg.write(hip + steering);

      if(steering > 28)
       rleg.write(hip + steering);
      
    }
    else{
         if(throttle >= 0){
         sequence = 3;
          }
        else{
          sequence = 1;
          }      
  }}
  
   if(sequence == 3)  // Rotatates bolth ankles to lift right foot 
  { 
    if(hip <= 135){ //was 115
     lleg.write(hip);
     rleg.write(hip);
     hip = hip + hipstep;
    }
    else{
         if(throttle >= 0){
          sequence = 0;
          }
        else{
          sequence = 2;
          }
}}}


