/*
  Simple clock using interrupts
  This clock works on 24 hour format
  Autor: Alex Suarez
  Licence: Beerware
  Contact: kapirucho@gmail.com
*/

#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int ledPin = 13;  // LED is attached to digital pin 13, LED included in board.
int x = 0;  // variable to be updated by the interrupt
int screen = 8, seconds = 0, minutes = 0, hours = 0;

const int up   = 22; //Pin reading
const int down = 23;
const int mode = 24;

int pushUp = 0, pushDown = 0, pushMode = 0, modo = 0;

void setup() {                
  //enable interrupt 4 (pin 19 on MEGA2560) which is connected to PWM pin 8
  attachInterrupt(4, increment, RISING);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);
  
  analogWrite(screen, 127); // Sending signal from PWM pin 8 to be read by interrup 4

  pinMode(up, INPUT);
  pinMode(down, INPUT);
  pinMode(mode, INPUT);
}

void loop() {
  digitalWrite(ledPin, LOW);
  modifyTime();
}

// Interrupt service routine for interrupt 4

void increment() {
  x++;
  if(x == 489) //As we are sending a signal of 127 this is half the work cycle so 489 pulses will be recorded as one second
  {
    x = 0;
    digitalWrite(ledPin, HIGH);
    seconds++;
    calcTime();

    lcd.clear();
    lcd.print(getsize(hours)+ ":" + getsize(minutes) + ":" + getsize(seconds));
  }
}

String getsize(int var)
{
  String va = String(var);
  if(va.length() <= 1)
  {
    return "0" + va;
  }
  else
  {
    return va;
  }
}

void modifyTime()
{
  pushMode = digitalRead(mode);

  if(pushMode == HIGH) //Selecting the "mode" between seconds, minutes, hours.
  {
    modo++;
    if(modo >= 3)
    {
      modo = 0;
    }
    lcd.print("Modo: " + String(modo));
    delay(300); // This helps avoid a "double" click on the push botton
  }

  if(digitalRead(up) == HIGH) //Add 1 to the specify variable
  {
    switch(modo)
    {
    case 0:
      segundos++;
      break;
    case 1:
      minutos++;
      break;
    case 2:
      horas++;
      break;
    }
    delay(300);
  }

  if(digitalRead(down) == HIGH) //Substract 1 to the specify variable
  {
    switch(modo)
    {
    case 0:
      segundos--;
      break;
    case 1:
      minutos--;
      break;
    case 2:
      horas--;
      break;
    }
    delay(300);
  }
}

void calcTime() //Simple time calcs
{
  if(seconds == 60) { 
    seconds = 0; 
    minutes++; 
  }

  if(minutes == 60) { 
    minutes = 0; 
    hours++; 
  }

  if(hours == 24) { 
    hours = 0; 
  }
}



