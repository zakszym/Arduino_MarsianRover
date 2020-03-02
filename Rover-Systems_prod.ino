/*
********************************************
Szymon Żak - Marsian-Rover-Systems v1.3
Git: https://github.com/zakszym/Arduino_MarsianRover.git
********************************************
This code describes Rover's sensor modules.  
Imperial March (Buzzer), 
Servo (Rover turret), Proximity sensor, Temp & Humidity 
********************************************
*/

/******************VARAS********************/
#define BLYNK_PRINT Serial  
#define TRIGGER_FRONT D1       //HC-SR04
#define ECHO_FRONT    D2       //HC-SR04
#define DHTPIN D8              //DHT11
#define DHTTYPE DHT11          //DHT11  
/*******************************************/

/******************LIBS*********************/
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <DHT.h>
/*******************************************/

/******************WIFI*********************/
char auth[] = "";
char ssid[] = "";
char pass[] = "";
/*******************************************/

/**************DTH11************************/
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V6, h);
  Blynk.virtualWrite(V7, t);
}
/*******************************************/

/*****************BUZZER********************/
  int speakerPin = D3;
  int length = 46;
  int playTime = 3;
  String notes[] {"A#4/Bb4","A#4/Bb4","A#4/Bb4","A#4/Bb4","A#4/Bb4","A#4/Bb4","A#4/Bb4","A#4/Bb4","A4","B4","C#5/Db5","C5","D#4/Eb4","D#4/Eb4","D#4/Eb4","D#4/Eb4","D#4/Eb4","D#4/Eb4","D#5/Eb5","D#5/Eb5","D5","D5","D5","E5","E5","F#4/Gb4","F#4/Gb4","F#5/Gb5","F5","G4","G4","G4","G4","G4","G4","G4","G4","G4","G4","G4","G5","G5","rest","rest","rest","rest"};
  int ordreNotes[]={4,7,13,16,33,35,41,44,34,32,30,31,3,6,15,37,40,43,12,25,9,10,11,24,26,14,39,22,23,0,1,2,5,8,17,19,20,28,42,45,18,21,27,29,36,38};
  int beats[] = { 8, 8, 8, 6, 2, 8, 6 , 2 ,16 , 8, 8, 8, 6, 2, 8, 6, 2, 16,8,6,2,8,6,2,2, 2, 2,6,2,2,8,6,2,2,2,2,6,2,2,9,6,2,8,6,2,16  };
  int tempo = 50;
  
  void playTone(int tone, int duration) {
    for (long i = 0; i < duration * 1000L; i += tone * 2) {
      digitalWrite(speakerPin, HIGH);
      delayMicroseconds(tone);
      digitalWrite(speakerPin, LOW);
      delayMicroseconds(tone);      
    }
  }
  
  void playNote(String note, int duration) {
    String noteNames[] = { "D#4/Eb4", "E4", "F4", "F#4/Gb4", "G4", "G#4/Ab4", "A4", "A#4/Bb4", "B4", "C5", "C#5/Db5", "D5", "D#5/Eb5", "E5", "F5", "F#5/Gb5", "G5", "G#5/Ab5", "A5", "A#5/Bb5", "B5", "C6", "C#6/Db6", "D6", "D#6/Eb6", "E6", "F6", "F#6/Gb6", "G6" };
    int tones[] = { 1607, 1516, 1431, 1351, 1275, 1203, 1136, 1072, 1012, 955, 901, 851, 803, 758, 715, 675, 637, 601, 568, 536, 506, 477, 450, 425, 401, 379, 357, 337, 318 };
    for (int i = 0; i < 29; i++) {
      if (noteNames[i] == note) {
        playTone(tones[i], duration);
      }
    }
  }

void playStarWars() {
      for (int i = 0; i < length; i++) {    
      
      if (notes[i] == "rest") {
        delay(beats[i] * tempo);
      } else {
        playNote(notes[tabOrdreNotesIndexSeartch(i)], beats[i] * tempo);
      }
      delay(tempo / 2);
    }
}
int tabOrdreNotesIndexSeartch(int value){
  int indexValue=0;
  while(indexValue<sizeof(ordreNotes) && ordreNotes[indexValue]!=value){
    indexValue++;
  }
  return indexValue;
}
/*******************************************/

/******************SG-90********************/
Servo servo;

BLYNK_WRITE(V8)
{
  servo.write(param.asInt());
}
/*******************************************/

/*******************************************/
void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  pinMode(TRIGGER_FRONT, OUTPUT);       //HC-SR04
  pinMode(ECHO_FRONT, INPUT);           //HC-SR04
  pinMode(BUILTIN_LED, OUTPUT);         //HC-SR04

  dht.begin();                          //DHT11
  timer.setInterval(1000L, sendSensor); //DHT11
  
  Serial.println("tests : debut");      //Buzzer
  Serial.println("tests : fin");        //Buzzer
  pinMode(speakerPin, OUTPUT);          //Buzzer
  
  servo.attach(2);                      //SG-90
}
/*******************************************/

/**************Play Star Wars***************/
BLYNK_WRITE(V13)
{
  int pinValue = param.asInt();  
  
  if (pinValue == 1) {
    for (int j = 0; j < playTime; j++) {
       playStarWars();
    }
  }

  Blynk.setProperty(V13, "label", pinValue);
  Serial.print(pinValue);
  Serial.println();
}
/*******************************************/

void loop()
{
  
/****************HC-SR04********************/
  long duration_f, distance_f;

  digitalWrite(TRIGGER_FRONT, LOW);

  delayMicroseconds(2);   

  digitalWrite(TRIGGER_FRONT, HIGH);

  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER_FRONT, LOW);
    
  duration_f = pulseIn(ECHO_FRONT, HIGH);
  distance_f = (duration_f/2) / 29.1;

   if (distance_f >= 50) {
    Blynk.virtualWrite(V0, 255);
}
  else {
    Blynk.virtualWrite(V0, 0);
  }

 if (distance_f >= 40) {
    Blynk.virtualWrite(V1, 255);
}
  else {
    Blynk.virtualWrite(V1, 0);
  }

   if (distance_f >= 30) {
    Blynk.virtualWrite(V2, 255);
}
  else {
    Blynk.virtualWrite(V2, 0);
  }

   if (distance_f >= 20) {
    Blynk.virtualWrite(V3, 255);
}
  else {
    Blynk.virtualWrite(V3, 0);
  }

   if (distance_f >= 10) {
    Blynk.virtualWrite(V4, 255);
}
  else {
    Blynk.virtualWrite(V4, 0);
  }

   if (distance_f < 9) {
    Blynk.virtualWrite(V4, 255);
    delay(500);
    Blynk.virtualWrite(V4, 0);
    delay(500);
}

  Serial.print(distance_f);
  Serial.println("Centimeter:");
  Blynk.virtualWrite(V5, distance_f);
  delay(200);
/*******************************************/

  Blynk.run();
  timer.run();

}
