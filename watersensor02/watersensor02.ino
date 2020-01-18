/* 
 * Water Sensor source code
 * author: roby
 * ver. : 1.1
 * date: 2020 01 15
 */

//
// sleep / powerdown header
#include <avr/sleep.h>

#define DEBUG
#ifdef DEBUG
#define print_debug( X ) Serial.println( X )
#else
#define print_debug( X )
#endif

//
// program version
const String currVersion = "v20200115";

  //
  // use 3.3V to ARef (be sure to measure it with a multimeter )
const double arefVoltage = 3.3;

const int NOTE_1 =  220;
const int NOTE_2 =  440;

 //
 // water voltage level
 //  if the voltage level < waterVoltage the sensor
 //  => water presence;
 //  tests with 220 Ohm + water sensor + 10K Ohm revealed
 //  - no water 1021 < read value < 1023
 //  - water           read value < 1020
const int waterVoltage = 1018;

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

  print_debug( "app ready ..." );
}


void ledBlink( int timeLen )
{
  digitalWrite(pinOutLED, HIGH);
  delay(timeLen);
  digitalWrite(pinOutLED, LOW);
  
  return;
}


bool checkForWaterLeakage()
{
  //
  // turn on sensor
  digitalWrite(pinOutH2O, HIGH);
  delay( 200 );
  
  //
  // read digital value from AD converter
  print_debug( "read analog value:" );

  int value = analogRead( A0 );

  print_debug( value );

  if( value <= waterVoltage )
  {
    print_debug( "Water detected on sensor" );
    return true;
  }

  //
  // turn off sensor
  digitalWrite(pinOutH2O, LOW);
  delay( 200 );

  return false;
}


//
// sleep every cycle
const int sleepLongTime = 20000;  // 20 secs
const int sleepShortTime = 3000;  //  3 secs


void loop() {

  //
  // led blink: I am alive
  ledBlink( 200 );

  int sleepActualTime = sleepLongTime;

  if( checkForWaterLeakage() )
  {
    //
    // water presence: sound alarm
    playAlarm( );

    //
    // in case of alarm shorten delay to next check
    sleepActualTime = sleepShortTime;
  }

  delay( sleepActualTime );
}