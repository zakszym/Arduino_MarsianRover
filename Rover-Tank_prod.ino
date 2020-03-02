/*
********************************************
Szymon Żak - Marsian-Rover-Systems v1.3
Git: https://github.com/zakszym/Arduino_MarsianRover.git
********************************************
This code describes Rover's maneuvers and concers blynk joystick 
and L298 H-Bridge module handling
********************************************
*/

/******************LIBS*********************/
#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
/*******************************************/

/******************LIBS*********************/
char auth[] = "";
char ssid[] = "";
char pass[] = "";
/*******************************************/

// These are used to set the direction of the bridge driver.
#define ENB D3      //ENB
#define MOTORB_1 D4 //IN3
#define MOTORB_2 D5 //IN4
#define MOTORA_1 D7 //IN1
#define MOTORA_2 D6 //IN2
#define ENA D8      //ENA

int motor_right_speed = 0;
int motor_left_speed = 0;


void setup()
{
  pinMode(ENA, OUTPUT);
  pinMode(MOTORA_1, OUTPUT);
  pinMode(MOTORA_2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(MOTORB_1, OUTPUT);
  pinMode(MOTORB_2, OUTPUT);

  digitalWrite(ENA,LOW);
  digitalWrite(ENB,LOW);
  
  Serial.begin(91200);


  Blynk.begin(auth, ssid, pass);
}

// JOYSTICK
BLYNK_WRITE(V0) {

  int nJoyY = param[0].asInt(); // read y-joystick
  int nJoyX = param[1].asInt(); // read x-joystick
  
  Serial.print("X = ");
  Serial.print(nJoyX);
  Serial.print("; Y = ");
  Serial.println(nJoyY);

  // OUTPUTS
  int nMotMixL; // Motor (left) 
  int nMotMixR; // Motor (right) 

  // CONFIG
  float fPivYLimit = 2046.0;
      
  // TEMP VARIABLES
  float   nMotPremixL;    // Motor (left) 
  float   nMotPremixR;    // Motor (right) 
  int     nPivSpeed;      // Pivot Speed
  float   fPivScale;      // Balance scale between drive and pivot

  // Calculate Drive Turn output due to Joystick X input
  if (nJoyY >= 0) {
    // Forward
    nMotPremixL = (nJoyX>=0)? 2046.0 : (2046.0 + nJoyX);
    nMotPremixR = (nJoyX>=0)? (2046.0 - nJoyX) : 2046.0;
  } else {
    // Reverse
    nMotPremixL = (nJoyX>=0)? (2046.0 - nJoyX) : 2046.0;
    nMotPremixR = (nJoyX>=0)? 2046.0 : (2046.0 + nJoyX);
  }

  // Scale Drive output due to Joystick Y input 
  nMotPremixL = nMotPremixL * nJoyY/2046.0;
  nMotPremixR = nMotPremixR * nJoyY/2046.0;

  nPivSpeed = nJoyX;
  fPivScale = (abs(nJoyY)>fPivYLimit)? 0.0 : (1.0 - abs(nJoyY)/fPivYLimit);

  // Calculate final mix of Drive and Pivot
  nMotMixL = (1.0-fPivScale)*nMotPremixL + fPivScale*( nPivSpeed);
  nMotMixR = (1.0-fPivScale)*nMotPremixR + fPivScale*(-nPivSpeed);
  
  motor_left_speed = nMotMixL;
  motor_right_speed = nMotMixR;
  Serial.print("M1 = ");
  Serial.print(motor_right_speed);
  Serial.print("; M2 = ");
  Serial.println(motor_left_speed);
 
  if (motor_right_speed > 1200) {
    digitalWrite(MOTORA_1,HIGH);
    digitalWrite(MOTORA_2,LOW);
  }
  else if (motor_right_speed < -1200) {
    digitalWrite(MOTORA_1,LOW);
    digitalWrite(MOTORA_2, HIGH);
    
  }
  else {
    digitalWrite(MOTORA_1, LOW);
    digitalWrite(MOTORA_2, LOW);
  }

  if (motor_left_speed > 1200) {
    digitalWrite(MOTORB_1, LOW);
    digitalWrite(MOTORB_2, HIGH);
  }
  else if (motor_left_speed < -1200) {
    digitalWrite(MOTORB_1,HIGH);
    digitalWrite(MOTORB_2,LOW);
  }
  else {
    digitalWrite(MOTORB_1, LOW);
    digitalWrite(MOTORB_2, LOW);
  }
  analogWrite(ENB, abs(motor_left_speed));
  analogWrite(ENA, abs(motor_right_speed));  
}

void loop()
{
  Blynk.run();
}
