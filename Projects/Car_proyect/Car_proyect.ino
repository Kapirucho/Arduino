#include "U8glib.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <NewPing.h>
#include <SPI.h>
#include <SD.h>

U8GLIB_ST7920_128X64_4X u8g(18, 16, 17);	// SPI Com: SCK = en = 18, MOSI = rw = 16, CS = di = 17

NewPing sonar(24, 25, 400);

int interrupt = 0;  // variable to be updated by the interrupt
int count = 40, seconds = 0, minutes = 0, hours = 0, cm = 0;
bool cardWorking = false, showWarning = false;
OneWire ds(45);
DallasTemperature sensors(&ds);

const int chipSelect = 53;
const __FlashStringHelper *temp;
String tempUno, tempDos;
void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  printTemp();
  printRango();
  printClock();
  printMessages();
  printWarning();
}

#define warning_sign_bits_width 30
#define warning_sign_bits_height 26
static unsigned char warning_sign_bits[] U8G_PROGMEM = {
   0x00, 0xc0, 0x00, 0x00, 0x00, 0xe0, 0x01, 0x00, 0x00, 0xe0, 0x01, 0x00,
   0x00, 0xf0, 0x03, 0x00, 0x00, 0x38, 0x07, 0x00, 0x00, 0x38, 0x07, 0x00,
   0x00, 0x1c, 0x0e, 0x00, 0x00, 0x1c, 0x0c, 0x00, 0x00, 0xce, 0x1c, 0x00,
   0x00, 0xe6, 0x19, 0x00, 0x00, 0xe7, 0x39, 0x00, 0x80, 0xe3, 0x71, 0x00,
   0x80, 0xe3, 0x71, 0x00, 0xc0, 0xc1, 0xe0, 0x00, 0xc0, 0xc0, 0xc0, 0x00,
   0xe0, 0xc0, 0xc0, 0x01, 0x70, 0xc0, 0x80, 0x03, 0x70, 0x00, 0x80, 0x03,
   0x38, 0x00, 0x00, 0x07, 0x18, 0xc0, 0x00, 0x06, 0x1c, 0xe0, 0x01, 0x0e,
   0x0e, 0xc0, 0x00, 0x1c, 0x0e, 0x00, 0x00, 0x1c, 0xff, 0xff, 0xff, 0x3f,
   0xff, 0xff, 0xff, 0x3f, 0xfe, 0xff, 0xff, 0x1f };

void printWarning(){
  int pos_y = 15;
  int pos_x = 13;
  if(showWarning){
    u8g.drawXBMP(pos_y,pos_x,warning_sign_bits_width,warning_sign_bits_height, warning_sign_bits);
    u8g.setFont(u8g_font_4x6);
    u8g.setPrintPos(pos_y + 2, 32 + pos_x);
    u8g.print("Peligro");
  }
}

void printMessages(){
  u8g.setFont(u8g_font_4x6);
  if(!cardWorking){
    u8g.setPrintPos(99, 54);
    u8g.print("No SD!");
  }
}

void printClock(){
  u8g.setFont(u8g_font_6x12);
  u8g.setPrintPos(99, 7);
  u8g.print(getsize(hours)+ ":" + getsize(minutes));
}

void printRango(){
  u8g.setFont(u8g_font_6x12);
  u8g.setPrintPos(0, 54);//Imprimir rango
  u8g.print(cm);
  if(cm <= 30){
    showWarning = true;
  } else {
    showWarning = false;
  }
}

void printTemp(){
  u8g.setFont(u8g_font_6x12);
  //Derecho
  u8g.setPrintPos(0, 64);
  u8g.print(tempUno);
  //Izquierdo
  u8g.setPrintPos(99, 64);
  u8g.print(tempDos);
}

String getsize(int var)
{
  String va = String(var);
  if(va.length() <= 1)
    return "0" + va;
  else
    return va;
}

void increment() {
  interrupt++;
  if(interrupt == 489) //As we are sending a signal of 127 this is half the work cycle so 489 pulses will be recorded as one second
  {
    interrupt = 0;
    seconds++;
    calcTime();
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

void setup(void) {
  Serial.begin(9600);
  sensors.begin();
  
  pinMode(22, INPUT);//min
  pinMode(23, INPUT);//sec
  attachInterrupt(4, increment, RISING);//enable interrupt 4 (pin 19 on MEGA2560) which is connected to PWM pin 8
  analogWrite(8, 127); // Sending signal from PWM pin 8 to be read by interrup 4
  // flip screen, if required
  u8g.setRot180();

  u8g.setColorIndex(1);
  
  pinMode(53, OUTPUT); //Pin for SD card
  
  // see if the card is present and can be initialized:
  if (SD.begin(chipSelect)) {
    cardWorking = true;
  }
}

void manageButtons(){
  if(digitalRead(22) == HIGH){
    minutes++;
    digitalWrite(13, HIGH);
  }
  if(digitalRead(23) == HIGH){
    hours++;
    digitalWrite(13, HIGH);
  }
  delay(300);
  digitalWrite(13, LOW);
}

void DoRange(){
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int uS = sonar.ping();
  cm = uS / US_ROUNDTRIP_CM;
}

void SaveData(String dataString){
  if(!cardWorking) return;
  
  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
}

void loop(void) {
  DoRange();
  manageButtons();
  if(count >= 40){
    sensors.requestTemperatures();
    tempUno = String(sensors.getTempCByIndex(0));
    tempDos = String(sensors.getTempCByIndex(1));
    SaveData(tempUno + ", " + tempDos + ", " + getsize(hours)+ ":" + getsize(minutes) + ":" + getsize(seconds));
    count = 0;
  }
  // picture loop
  u8g.firstPage();
  do {
    draw();
  } while( u8g.nextPage() );
  // rebuild the picture after some delay
  delay(100);
  count++;
}

