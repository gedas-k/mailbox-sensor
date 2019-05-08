#include <avr/sleep.h>
#include <avr/interrupt.h>

// Adjust this part**********************************************
#define sensorPin A1            // select the input pin for the potentiometer
#define IRPinOne 3              // select the pin for the IR LED
#define IRPinTwo 4              // select the pin for the IR LED
#define ledPin 0                // select the pin for the mail delivery LED
#define buttonPin 1             // select button pin

#define brightness 1            // select mail delivery LED brightness (0-255)
#define sensitivity 120         // sensor sensitivity (set by trial and error)
#define CheckTimeIntervals 60   // set check intervals in minutes
//**************************************************************

// Routines to set and claer bits (used in the sleep code)
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Variables for the Sleep/power down modes:
volatile boolean f_wdt = 1;

int sensorValue, sensorValueH, sensorValueL;  // variable to store the value coming from the sensor
bool mail = false;      // is mail receaved

void setup() {
  setup_watchdog(9); // Setup WatchDog
  IOpins(); // Set pins to input for less energy consumption
  int timeMultiplyer = CheckTimeIntervals * 60 / 8; // Change sleep time to 8s interval count
}

void loop() {

  if (f_wdt==1) {  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
    f_wdt=0;       // reset flag

    //pinMode(ledPin, INPUT);
    while(mail == false){

      for(int i = 0; i < timeMultiplyer; i++){ // Multiply WatchDog time for longer sleep
        system_sleep();  // Send the unit to sleep

        // Check if button is preesed for batteries check
        if (buttonState == HIGH){
          analogWrite(ledPin, brightness);
          delay(3000);
          analogWrite(ledPin, LOW);
        }
      }

      getSensor(); //read the value from the sensor
    }

    IOpins();
    pinMode(ledPin, OUTPUT);
    analogWrite(ledPin, brightness);
    pinMode(buttonPin, INPUT);

    int buttonState;

    bool removed = false;
    while(removed == false){
      buttonState = digitalRead(buttonPin); // remove mail
      if (buttonState == HIGH){
        analogWrite(ledPin, LOW);
        IOpins();
        removed = true;
        mail = false;
      }
    }
  }

}

void getSensor() {
  pinMode(IRPinOne, OUTPUT);
  pinMode(IRPinTwo, OUTPUT);
  int IRPin[] = {IRPinOne, IRPinTwo};
  for(int i = 0; i < 2; i++){
    digitalWrite(IRPin[i], HIGH);
    delay(10);
    sensorValueH = analogRead(sensorPin); //read whit IR LED
    digitalWrite(IRPin[i], LOW);
    delay(10);
    sensorValueL = analogRead(sensorPin); //read witouth IR LED

    sensorValue = sensorValueH - sensorValueL; //is it changed
    sensorValue = abs(sensorValue); // removes minus if egsists

    //Condition for delivered mail:
    if(sensorValue < sensitivity){
      mail = true;
    }
  }
  IOpins();
}

void IOpins(){
  //Set all pins to inputs w/ pull-up resistors (stop them from floating or sourcing power)
  for (byte i=0; i<5; i++) {
    pinMode(i, INPUT);
    //digitalWrite(i, LOW);
  }
}

/*---------------------------------------------------
 * Sleep functions:
 */

// set system into the sleep state
// system wakes up when wtchdog is timed out
void system_sleep() {

  cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System actually sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out

  sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON

}

// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {

  byte bb;
  int ww;
  if (ii > 9 ) ii=9;
  bb=ii & 7;
  if (ii > 7) bb|= (1<<5);
  bb|= (1<<WDCE);
  ww=bb;

  MCUSR &= ~(1<<WDRF);
  // start timed sequence
  WDTCR |= (1<<WDCE) | (1<<WDE);
  // set new watchdog timeout value
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt=1;  // set global flag
}
