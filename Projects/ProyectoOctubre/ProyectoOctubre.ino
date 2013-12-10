int ojoIzq = 9, ojoDer = 8;
int verdeUno = 3, verdeDos = 4, verdeTres = 5, verdeCuatro = 6;

bool led = false;
int aleatorio = 100, contador = 0;
int verdeBright = 0, brightness = 0, fadeamount = 5;
int sensorIzq = 0, sensorDer = 0;

bool increase = false, start = true;

int x= 0, seconds = 0, minutes = 0;


// the setup routine runs once when you press reset:
void setup()  {
  Serial.begin(9600);
  attachInterrupt(4, increment, RISING);//Interrupt for internal clock PIN 19 MEGA2560
  analogWrite(7, 127); // Sending signal from PWM pin 8 to be read by interrup 4
  pinMode(13, OUTPUT);
  pinMode(ojoIzq, OUTPUT);
  pinMode(ojoDer, OUTPUT);
  pinMode(verdeUno, OUTPUT);
  pinMode(verdeDos, OUTPUT);
  pinMode(verdeTres, OUTPUT);
  pinMode(verdeCuatro, OUTPUT);
}

void increment() {
  x++;
  if(x == 489) //As we are sending a signal of 127 this is half the work cycle so 489 pulses will be recorded as one second
  {
    x = 0;
    seconds++;
    contador++;
    calcTime();
    if(led) {
      digitalWrite(13, HIGH); led = false;
    } else { digitalWrite(13, LOW); led = true; }
  }
  if(contador >= 5) { contador = 0; aleatorio = random(10, 255); }
}

void calcTime() //Simple time calcs
{
  if(seconds == 60) { 
    seconds = 0; 
    minutes++;
    start = !start;
  }
  //Serial.println("Tiempo: " + String(minutes) + ":" + String(seconds) + ", Distancia; Izq: " + String(sensorIzq) + " Der: " + String(sensorDer));
}

void loop()  {
  analogWrite(ojoIzq, brightness);
  analogWrite(ojoDer, brightness);
  analogWrite(verdeUno, verdeBright);
  analogWrite(verdeDos, verdeBright);
  analogWrite(verdeTres, verdeBright);
  analogWrite(verdeCuatro, verdeBright);
  
  doOjoFade();
  doVerdeFade();
}

void doOjoFade(){
  if(increase){
    if(aleatorio < brightness){
      brightness = brightness - fadeamount;
    }
    if(aleatorio > brightness){
      brightness = brightness + fadeamount;
    }
    delay(30);
  }
  
  if(aleatorio == brightness){
    increase = false;
  } else {
    increase = true;
  }
}

void doVerdeFade(){
  if(start){
    if(verdeBright < 255){
      verdeBright = verdeBright + fadeamount;
      delay(30);
    }
  } else {
    if(verdeBright > 0){
      verdeBright = verdeBright - fadeamount;
      delay(30);
    }
  }
}
