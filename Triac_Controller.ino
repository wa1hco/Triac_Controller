#include "TimerOne.h"
#include "TimerThree.h"
#include <LiquidCrystal.h>


// connections for "Teensy Everything Box"
#define    LCDRsPin   16
#define    LCDEnPin   17
#define    LCDD7Pin   24
#define    LCDD6Pin   26
#define    LCDD5Pin   28
#define    LCDD4Pin   30
#define   LCDDimPin    4
LiquidCrystal lcd(LCDRsPin,
                  LCDEnPin,
                  LCDD4Pin,
                  LCDD5Pin,
                  LCDD6Pin,
                  LCDD7Pin);

#define LEDPIN 20
#define PotTopPin A11
#define PotBotPin A10

unsigned long time1;
unsigned long lastTime1 = 0;
unsigned long dTimer1;
unsigned long time3;
unsigned long dTime;

float         zero_crossing_time = 1.0/120.0; //seconds
unsigned int  time_us = (unsigned int) (zero_crossing_time * 1e6);

float         duty = 0.5;
unsigned int  delay_us = (unsigned int) ((float)(time_us) * duty);

int           inChar;
int           inValue;
String        inString = "";;

void setup() 
{
  // setup code
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial) 
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("time ");
  Serial.print(time_us);
  Serial.print(", delay_us ");
  Serial.print(delay_us);
  Serial.print("\n");

  pinMode(PotTopPin, INPUT);

  lcd.begin(20, 4);
  lcd.home();
  lcd.clear();
  
  // repeated interrupts at line zero crossing interval
  Timer1.initialize(time_us); // 8.333 ms, 120 Hz
  Timer1.attachInterrupt(T1_int);

  // one shot timer interrupt for triac trigger
  Timer3.initialize();
  Timer3.stop();
  Timer3.attachInterrupt(T3_int);
}

// simulation of zero crossing interrupt
void T1_int()
{
  time1 = micros();            // first, grab the current time
  Timer3.setPeriod(delay_us * 2);  // reset the timer
  Timer3.start();
  digitalWrite(LEDPIN, 0);     // turn off the light
  dTimer1 = time1 - lastTime1;
  lastTime1 = time1;
}

void T3_int()
{
  time3 = micros();
  Timer3.stop();
  digitalWrite(LEDPIN, 1);
  dTime = time3 - time1;
}
void loop() 
{  
  int d1, d2, d3;

  inValue = 8 * analogRead(PotTopPin);
//  Serial.print("inValue = ");
//  Serial.print(inValue);
//  Serial.print("\n");

  // Interpret input to delay in usec
  noInterrupts();
  delay_us = inValue;
  interrupts();
  
  delay(30);
  // get the two values managed in interrupts
  noInterrupts();
  d1 = delay_us;
  d2 = dTime;
  d3 = dTimer1;
  interrupts();

  lcd.setCursor(0, 0);
  lcd.print("delay_us = ");
  lcd.print(d1);
  lcd.print("    ");
  lcd.setCursor(0, 1);
  lcd.print("dTimer1 = ");
  lcd.print(d3);
  lcd.print("    ");
  lcd.setCursor(0,2);
  lcd.print("dTime = ");
  lcd.print(d2);
  lcd.print("    ");


} // loop()
