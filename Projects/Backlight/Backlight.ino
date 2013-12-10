

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

const int buttonPinDown = 24;     // the number of the pushbutton pin
const int buttonPinUp = 25;

const int ledPin =  13;      // the number of the LED pin
const int ScreenPin = 8; // Pin para pantalla

// variables will change:
int buttonStateUp = 0;         // variable for reading the pushbutton status
int buttonStateDown = 0;         // variable for reading the pushbutton status

int contador = 0, backLight = 0, porcentaje = 0, funcion = 0;

boolean func = false;
boolean screenon = true;

void setup() {
  lcd.begin(16, 2);

  pinMode(ledPin, OUTPUT);      
  // initialize the pushbutton pin as an input:
  pinMode(buttonPinUp, INPUT); 
  pinMode(buttonPinDown, INPUT); 
  
  InitBacklight();
}

void loop() {
  
  Backlight();
}

void InitBacklight()
{
  backLight = 128;  
  porcentaje = (backLight * 100) / 255;

  analogWrite(ScreenPin, backLight);
  // Print a message to the LCD.
  lcd.print("Welcome! Backlight dim test.");
  lcd.setCursor(0,1);
  lcd.print("BL: " + String(porcentaje) + "% - " + String(backLight));
  delay(2000);
}

void Backlight()
{
  buttonStateUp = digitalRead(buttonPinUp);
  buttonStateDown = digitalRead(buttonPinDown);

  if(backLight >= 255)  { backLight = 255; }

  if(buttonStateUp == HIGH)
  {
    digitalWrite(ledPin, HIGH);
    
    if(backLight >= 0 && screenon == false)
    {
      lcd.display();
      screenon = true;
    }
    
    backLight += 10;
    if(backLight >= 255) { backLight = 255; }
    
    func = true;
  }

  if (buttonStateDown == HIGH)
  {
    digitalWrite(ledPin, HIGH);
    backLight -= 10;
    
    if(backLight <= 0) { backLight = 0; }

    if(backLight <= 0 && screenon == true)
    {
      lcd.noDisplay();
      screenon = false;
    }
    
    func = true;
  }

  if(func)
  {
    lcd.clear();
    porcentaje = (backLight * 100) / 255;
    lcd.setCursor(0,0);
    lcd.print("Modify backlight.");
    lcd.setCursor(0,1);
    lcd.print("BL: " + String(porcentaje) + "% - " + String(backLight));
    digitalWrite(ledPin, LOW);
    analogWrite(Pantalla, backLight);
    func = false;
    delay(200);
  }
}


