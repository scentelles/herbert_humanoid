
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include "MqttConnection.h"



/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "SFR_34A8"
#define WLAN_PASS       "ab4ingrograstanstorc"



MqttConnection * myMqtt;
String tmpString;

int currentServo = 0;
int currentValue = 300;

#define TEMPO 100


#define NB_PARTS 12
#define LEFT_FOOT 0
#define LEFT_KNEE 1
#define LEFT_LEG  2

#define RIGHT_FOOT 4
#define RIGHT_KNEE 5
#define RIGHT_LEG  6

#define LEFT_ARM_UP1 8
#define LEFT_ARM_UP2 9
#define LEFT_ARM_ELBOW 10

#define RIGHT_ARM_UP1    12
#define RIGHT_ARM_UP2    13
#define RIGHT_ARM_ELBOW  14

float positionArray[20];
float moveArray[NB_PARTS];

int partIndexToId[NB_PARTS];

#define MIN_SERVO_VALUE 150
#define MAX_SERVO_VALUE 600




// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // this is the 'maximum' pulse length count (out of 4096)

/************************* MQTT *********************************/

#define MQTT_SERVER  "192.168.1.27"
#define MQTT_PORT 1883



void initPosition(){
  Serial.println("Init robot position");
  
  positionArray[LEFT_FOOT] = 240;
  pwm.setPWM(LEFT_FOOT, 0, positionArray[LEFT_FOOT]);
  positionArray[LEFT_KNEE] = 350;
  pwm.setPWM(LEFT_KNEE, 0, positionArray[LEFT_KNEE]);
  positionArray[LEFT_LEG] = 260;
  pwm.setPWM(LEFT_LEG, 0, positionArray[LEFT_LEG]);

  positionArray[RIGHT_FOOT] = 290;
  pwm.setPWM(RIGHT_FOOT, 0, positionArray[RIGHT_FOOT]);
  positionArray[RIGHT_KNEE] = 345;
  pwm.setPWM(RIGHT_KNEE, 0, positionArray[RIGHT_KNEE]);
  positionArray[RIGHT_LEG] = 360;
  pwm.setPWM(RIGHT_LEG, 0, positionArray[RIGHT_LEG]);  


  positionArray[LEFT_ARM_UP1] = 330;
  pwm.setPWM(LEFT_ARM_UP1, 0, positionArray[LEFT_ARM_UP1]);
  positionArray[LEFT_ARM_UP2] = 170;
  pwm.setPWM(LEFT_ARM_UP2, 0, positionArray[LEFT_ARM_UP2]);
  positionArray[LEFT_ARM_ELBOW] = 360;
  pwm.setPWM(LEFT_ARM_ELBOW, 0, positionArray[LEFT_ARM_ELBOW]);  
  
  positionArray[RIGHT_ARM_UP1] = 320;
  pwm.setPWM(RIGHT_ARM_UP1, 0, positionArray[RIGHT_ARM_UP1]); 
  positionArray[RIGHT_ARM_UP2] = 470;
  pwm.setPWM(RIGHT_ARM_UP2, 0, positionArray[RIGHT_ARM_UP2]); 

//TODO : add right elbow


}



void movePart(int part, float value){

  float newPosition = positionArray[part] + value;
  Serial.println(newPosition);
  if(newPosition < MIN_SERVO_VALUE){
      newPosition = MIN_SERVO_VALUE;
  }
  if(newPosition > MAX_SERVO_VALUE){
      newPosition = MAX_SERVO_VALUE;
  }
  Serial.print("Moving part : ");
  Serial.println(part);
  Serial.print("    ");
  Serial.println(newPosition);
   pwm.setPWM(part, 0, newPosition); 
 //store new position
  positionArray[part] = newPosition;
 
}
  
void moveByVector(float * moveArray, int nbSteps){
  Serial.println("moving by vector");
  for(int i = 0 ; i < nbSteps; i++){
       yield(); // to avoid WDT to trigger
      // Serial.println(i);
       for(int partIndex = 0; partIndex < NB_PARTS; partIndex++){
           int thisPart = partIndexToId[partIndex];
           float increment = moveArray[partIndex] / nbSteps; 
           movePart(thisPart, increment);
       }
    
  }
  
}



void initWalkMove(){
 
  moveArray[0] = 0;
  moveArray[1] = 0;
  moveArray[2] = 0;
  moveArray[3] = 0;
  moveArray[4] = 0;
  moveArray[5] = 50;
  moveByVector(moveArray, 100);

}


void getUpMove(){

  initPosition();
  
  moveArray[0] = 0;
  moveArray[1] = 210;
  moveArray[2] = 0;
  moveArray[3] = 0;
  moveArray[4] = -210;
  moveArray[5] = 0;
  moveByVector(moveArray, 7);

  moveArray[0] = 60;
  moveArray[1] = 0;
  moveArray[2] = -120;
  moveArray[3] = 0;
  moveArray[4] = 0;
  moveArray[5] = 0;
  moveByVector(moveArray, 5);

  //delay(500);

  moveArray[0] = -70;
  moveArray[1] = 0;
  moveArray[2] = 0;
  moveArray[3] = 0;
  moveArray[4] = 0;
  moveArray[5] = 240;
  moveByVector(moveArray, 7);


  moveArray[0] = 0;
  moveArray[1] = 0;
  moveArray[2] = 0;
  moveArray[3] = 0;
  moveArray[4] = 40;
  moveArray[5] = 0;
  moveByVector(moveArray, 4);

  initPosition();
}

void walkMove(){

  moveArray[0] = 5;
  moveArray[1] = 0;
  moveArray[2] = 0;
  moveArray[3] = 25;
  moveArray[4] = 0;
  moveArray[5] = 0;
  moveByVector(moveArray, 5);


  for(int i = 0; i < 10; i++){

  
  moveArray[0] = 0;
  moveArray[1] = -40;
  moveArray[2] = -60;
  moveArray[3] = 0;
  moveArray[4] = 0;
  moveArray[5] = 10;
 // moveArray[6] = 20;
  moveByVector(moveArray, 7);

  moveArray[0] = 0;
  moveArray[1] = 0;
  moveArray[2] = 40;
  moveArray[3] = -10;
  moveArray[4] = -40;
  moveArray[5] = -30;
//  moveArray[6] = 20;
  moveByVector(moveArray, 5);
        
  moveArray[0] = -10;
  moveArray[1] = 0;
  moveArray[2] = -20;
  moveArray[3] = -10;
  moveArray[4] = -15;
  moveArray[5] = -10;
//  moveArray[6] = 5;
  moveByVector(moveArray, 2);
  
//#4
  moveArray[0] = 0;
  moveArray[1] = 30;
  moveArray[2] = 20;
  moveArray[3] = -10;
  moveArray[4] = 0;
  moveArray[5] = 0;
//  moveArray[6] = -5;
  moveByVector(moveArray, 3); 

//#5
  moveArray[0] = -10;
  moveArray[1] = -15;
  moveArray[2] = 10;
  moveArray[3] = 10;
  moveArray[4] = 85;
  moveArray[5] = 100;
//  moveArray[6] = -20;
  moveByVector(moveArray, 7); 


  moveArray[0] = 10;
  moveArray[1] = 0;
  moveArray[2] = 0;
  moveArray[3] = 0;
  moveArray[4] = -30;
  moveArray[5] = -70;
//  moveArray[6] = -20;
  moveByVector(moveArray, 5); 
 
  moveArray[0] = 10;
  moveArray[1] = 25;
  moveArray[2] = 10;
  moveArray[3] = 20;
  moveArray[4] = 0;
  moveArray[5] = 0;
  moveByVector(moveArray, 2); 
  
  }
  
}


void customMove2(){
  float moveArray[NB_PARTS];
  moveArray[0] = 0;
  moveArray[1] = 0;
  moveArray[2] = -30;
  moveArray[3] = 0;
  moveArray[4] = 0;
  moveArray[5] = 30;
  moveByVector(moveArray, 30);
}

void walk(){
  
  pwm.setPWM(LEFT_FOOT, 0, 220);
  delay(TEMPO);
  pwm.setPWM(LEFT_KNEE, 0, 370);
  delay(TEMPO);
  pwm.setPWM(LEFT_LEG, 0, 310);
  delay(TEMPO);
  pwm.setPWM(RIGHT_FOOT, 0, 270);
  delay(TEMPO);
  pwm.setPWM(RIGHT_KNEE, 0, 400);
  delay(TEMPO);
  pwm.setPWM(RIGHT_LEG, 0, 400); 
  delay(TEMPO);  
  pwm.setPWM(LEFT_FOOT, 0, 255);  
  delay(TEMPO); 
  pwm.setPWM(RIGHT_FOOT, 0, 320); 
  delay(TEMPO);
  pwm.setPWM(RIGHT_KNEE, 0, 350);
  delay(TEMPO);
  pwm.setPWM(RIGHT_LEG, 0, 380);  
  delay(TEMPO); 
  pwm.setPWM(LEFT_FOOT, 0, 260);
  delay(TEMPO);
  pwm.setPWM(RIGHT_FOOT, 0, 320); 
  delay(TEMPO);
  pwm.setPWM(RIGHT_KNEE, 0, 320);  
  delay(TEMPO);
  pwm.setPWM(RIGHT_LEG, 0, 330);   
  delay(TEMPO); 
  pwm.setPWM(LEFT_LEG, 0, 250);
  delay(TEMPO);
  pwm.setPWM(RIGHT_FOOT, 0, 270);
  delay(TEMPO);
  pwm.setPWM(LEFT_KNEE, 0, 320);
  delay(TEMPO);
  
  
}


void processCommandMsg(char* topic, byte* payload, unsigned int length)
{
      int currentValue = 0;
      char localPayload[20];
      memset(localPayload, 0, 20);
      memcpy(localPayload, payload, length); 
      if(String(topic) == "ROBOT/LF"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting LF Servo :");
        Serial.println(currentValue);
        pwm.setPWM(0, 0, currentValue);
      }
      if(String(topic) == "ROBOT/LK"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting LK Servo :");
        Serial.println(currentValue);
        pwm.setPWM(1, 0, currentValue);
      }     
      if(String(topic) == "ROBOT/LL"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting LL Servo :");
        Serial.println(currentValue);
        pwm.setPWM(2, 0, currentValue);
      }
      if(String(topic) == "ROBOT/RF"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting RF Servo :");
        Serial.println(currentValue);
        pwm.setPWM(4, 0, currentValue);
      }
      if(String(topic) == "ROBOT/RK"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting RK Servo :");
        Serial.println(currentValue);
        pwm.setPWM(5, 0, currentValue);
      }
      if(String(topic) == "ROBOT/RL"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting RL Servo :");
        Serial.println(currentValue);
        pwm.setPWM(6, 0, currentValue);

      }  
      if(String(topic) == "ROBOT/LA_UP1"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting LA UP1 Servo :");
        Serial.println(currentValue);
        pwm.setPWM(8, 0, currentValue);

      }  
      if(String(topic) == "ROBOT/LA_UP2"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting LA UP2 Servo :");
        Serial.println(currentValue);
        pwm.setPWM(9, 0, currentValue);

      }  
      if(String(topic) == "ROBOT/LA_ELBOW"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting LA UP1 Servo :");
        Serial.println(currentValue);
        pwm.setPWM(10, 0, currentValue);

      }       
      if(String(topic) == "ROBOT/RA_UP1"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting LA UP1 Servo :");
        Serial.println(currentValue);
        pwm.setPWM(12, 0, currentValue);

      }  
      if(String(topic) == "ROBOT/RA_UP2"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting LA UP2 Servo :");
        Serial.println(currentValue);
        pwm.setPWM(13, 0, currentValue);

      }  
      if(String(topic) == "ROBOT/RA_ELBOW"){
        currentValue = String(localPayload).toInt();
        Serial.print("setting LA UP1 Servo :");
        Serial.println(currentValue);
        pwm.setPWM(14, 0, currentValue);

      } 



      
      if(String(topic) == "ROBOT/init"){


  positionArray[LEFT_FOOT] = 240;
  positionArray[LEFT_KNEE] = 350;
  positionArray[LEFT_LEG] = 260;

  positionArray[RIGHT_FOOT] = 290;
  positionArray[RIGHT_KNEE] = 345;
  positionArray[RIGHT_LEG] = 360;

  positionArray[LEFT_ARM_UP1] = 330;
  positionArray[LEFT_ARM_UP2] = 170;
  positionArray[LEFT_ARM_ELBOW] = 360;
 
  positionArray[RIGHT_ARM_UP1] = 320;
  positionArray[RIGHT_ARM_UP2] = 470;
  
        String tmpString1 = "240";
        myMqtt->publishValue("LF", tmpString1.c_str());
                myMqtt->loop();
        String tmpString2 = "350";
        myMqtt->publishValue("LK", tmpString2.c_str());
                myMqtt->loop();
        String tmpString3 = "260";
        myMqtt->publishValue("LL", tmpString3.c_str());
                myMqtt->loop();
        String tmpString4 = "290";
        myMqtt->publishValue("RF", tmpString4.c_str());
                myMqtt->loop();
        String tmpString5 = "345";
        myMqtt->publishValue("RK", tmpString5.c_str());
                myMqtt->loop();
        String tmpString6 = "360";
        myMqtt->publishValue("RL", "350");
                myMqtt->loop();





        Serial.println("========================== INIT =========================");

      }
      if(String(topic) == "ROBOT/walk"){
            walkMove();
      }

      if(String(topic) == "ROBOT/getup"){
            getUpMove();
      }
      
      if(String(topic) == "ROBOT/debug"){
        moveArray[0] = 10;
  moveArray[1] = -40;
  moveArray[2] = -50;
  moveArray[3] = 30;
  moveArray[4] = 0;
  moveArray[5] = 10;
  moveByVector(moveArray, 30);
      }
}


void setup() {
  Serial.begin(115200);
  Serial.println("16 channel Servo test!");

  pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

partIndexToId[0] = LEFT_FOOT;
partIndexToId[1] = LEFT_KNEE;
partIndexToId[2] = LEFT_LEG;
partIndexToId[3] = RIGHT_FOOT;
partIndexToId[4] = RIGHT_KNEE;
partIndexToId[5] = RIGHT_LEG;


  myMqtt = new MqttConnection("ROBOT", WLAN_SSID, WLAN_PASS, MQTT_SERVER, MQTT_PORT);
  myMqtt->registerCustomProcessing(&processCommandMsg);
            myMqtt->addSubscription("LF");
            myMqtt->addSubscription("LK");
            myMqtt->addSubscription("LL");
            myMqtt->addSubscription("RF");
            myMqtt->addSubscription("RK");
            myMqtt->addSubscription("RL");
            myMqtt->addSubscription("LA_UP1");
            myMqtt->addSubscription("LA_UP2");
            myMqtt->addSubscription("LA_ELBOW");
            myMqtt->addSubscription("RA_UP1");
            myMqtt->addSubscription("RA_UP2");
            myMqtt->addSubscription("RA_ELBOW");
            myMqtt->addSubscription("init");
            myMqtt->addSubscription("walk");
            myMqtt->addSubscription("getup");
            myMqtt->addSubscription("debug");
 // if (!myMqtt->connected()) {
 //   myMqtt->reconnect();
 // }
 //   delay(1000);
   initPosition();
    
 // initWalkMove();
  yield();


  
}

void loop() {

  if (!myMqtt->connected()) {
    myMqtt->reconnect();
  }
  myMqtt->loop();


 // delay(TEMPO);

  //walkMove();



}

