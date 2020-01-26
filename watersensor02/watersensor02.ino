/* 
 * Water Sensor source code
 * author: roby
 * ver. : 1.3
 * date: 2020 01 26
 */

//
// sleep / powerdown header
#include "LowPower.h"

//
// enable debug printout
#define DEBUG 1

#ifdef DEBUG
//
// enable debug print
// note: a delay of 200 msec is used to allow the console
//       to finish the output of message
#define print_debug( X ) do {                      \
                              Serial.println( X ); \
                              delay(200);          \
                         } while(0)
#else
//
// disable debug print
#define print_debug( X )

#endif

//
// program version
const String currVersion = "v20200126";

//
// water voltage level
//  if the voltage level < waterVoltage the sensor
//  => water presence;
//  tests with 220 Ohm + water sensor + 10K Ohm revealed
//  - no water 1021 < read value < 1023
//  - water           read value < 1020
const int waterVoltage = 1020;

const int NOTE_1 =  220;
const int NOTE_2 =  440;

//
// piezo pin out
const int pinOut = 8;
const int pinOutH2O = 2;
const int pinOutLED = 13;



void playTone( int note, int duration )
{
  tone( pinOut, note, duration ); 
  delay( duration );
  noTone( pinOut );
  
  return;
}


void playAlarm()
{
  const int duration = 400;
  playTone( NOTE_2, duration );
  playTone( NOTE_1, duration );
  playTone( NOTE_2, duration );
}


void setup() {

  //
  // init console
  Serial.begin( 9600 );
  print_debug( "water sensor app starting ..." );
  print_debug( currVersion );

  //
  // If you want to set the aref to something other than 5v
  // Not working on arduino pro mini
  //analogReference(EXTERNAL);

  //
  // PinOut: initialize as an output.
  print_debug( "set speaker pin output ..." );
  print_debug( pinOut );
  pinMode( pinOut, OUTPUT);

  //
  // PinOut 2 : initialize as an output.
  print_debug( "set H2O sensor pin output ..." );
  print_debug( pinOutH2O );
  pinMode( pinOutH2O, OUTPUT);
  digitalWrite(pinOutH2O, LOW);

  //
  // turn off led: initialize digital pin 13 as an output.
  print_debug( "turn off LED ..." );
  print_debug( pinOutLED );
  pinMode(pinOutLED, OUTPUT);
  digitalWrite(pinOutLED, LOW);

  //
  // make sound: I am starting ...
  print_debug( "testing sound ..." );
  playAlarm();

  print_debug("threashold value :");
  print_debug( waterVoltage );

  print_debug( "app ready ..." );
}



void ledBlink( int timeLen ) {
  digitalWrite(pinOutLED, HIGH);
  delay(timeLen);
  digitalWrite(pinOutLED, LOW);
  
  return;
}



bool checkForWaterLeakage() {
  //
  // turn on sensor
  digitalWrite(pinOutH2O, HIGH);
  delay( 500 );
  
  //
  // read digital value from AD converter
  print_debug( "read analog value from sensor:" );

  int value = analogRead( A0 );

  print_debug( value );

  bool alarmCondition = false;
    
  if( value < waterVoltage ) {
    print_debug( "alarm: water detected on sensor" );
    alarmCondition =  true;
  }

  //
  // turn off sensor
  digitalWrite(pinOutH2O, LOW);
  delay( 200 );
  
  return alarmCondition;
}



void suspendDevice(const period_t sleepPeriod, const int periodCount) {
    //
    // go into long low-power mode
    print_debug( "low-power mode ..." );

    for(int i = 0; i < periodCount; i++) {
      //
      // enter power down state with ADC and BOD module disabled
      LowPower.powerDown(sleepPeriod, ADC_OFF, BOD_OFF);
    }
    delay(200);

    print_debug( "resuming from low-power mode ..." );

  return ;
}



void loop() {

  //
  // led blink: I am alive
  ledBlink( 200 );

  //
  // check water sensor
  bool alarmCondition = checkForWaterLeakage();
  
  if( alarmCondition ) {
    //
    // water presence: sound alarm
    playAlarm( );

    //
    // alarm condition: go into short low-power mode (about 2sec.)
    suspendDevice(SLEEP_2S, 1 /* period(s) count */);
    
  } else {
    //
    // no alarm, just go into long low-power mode (about 32sec.)
    suspendDevice(SLEEP_8S, 4 /* period(s) count */);
  }

  //
  // end of loop()
}
