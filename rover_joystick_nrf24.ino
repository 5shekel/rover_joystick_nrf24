/**
* of how to use the RF24 class to control a 2 DC motor bot
* https://github.com/5shekel/rover_joystick_nrf24

* control remote bot 
* based on  RF24 lib Example http://tmrh20.github.io/RF24/pingpair_dyn_8ino-example.html 
* for clrifing the array sending see http://forum.arduino.cc/index.php?topic=260853.0
*/

#define DEBUG_PONG 0

#include "RF24.h" //http://tmrh20.github.io/ this is a manitned fork, the original is now radiohead
#include <SPI.h>
RF24 radio(9,8);

const int role_pin = 4;// sets the role of this unit in hardware.  
// The various roles supported by this sketch
typedef enum { role_remote, role_bot } role_e;
// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Remote", "Bot"};
role_e role;// The role of the current running sketch
byte addresses[][6] = {"1Node","2Node"};

#include <Bounce2.h> // https://github.com/thomasfredericks/Bounce-Arduino-Wiring
#include "Streaming.h"

//motor pins
///////// motor /////////////
int inApin[2] = {A1, A3};  // INA: Clockwise input
int inBpin[2] = {A2, A4}; // INB: Counter-clockwise input
int pwmpin[2] = {3, 6}; // PWM input
#define stdbyMotorpin A5
/// neck pin
#include <Servo.h>
Servo neckServo;
int neckPwmPin = 5;
#define neck_down_pin 2
#define neck_up_pin 3
#define neck_midpoint 67
#define neck_speed 30

#define BTN_pin 7
#define AX_pin A0
#define AY_pin A1
#define joystickNoise 10
#define joystickXmid 482
#define joystickYmid 498

struct dataStruct{
  int xout;
  int yout;
  int btn;
  int neck;
}myData;
int x2pwm, y2pwm ;

Bounce bouncer = Bounce();

void setup(void)
{
  // set up the role pin
  pinMode(role_pin, INPUT);
  digitalWrite(role_pin, HIGH);
  delay(20); // Just to get a solid reading on the role pin

  // read the address pin, establish our role
  if ( digitalRead(role_pin) )
  role = role_remote;
  else
  role = role_bot;

  Serial.begin(57600);
  Serial<<"\n\rYAWCB/\n\r ROLE:"<< role_friendly_name[role]<<endl;

  radio.begin();

  if ( role == role_bot ) {  // Start listening
   radio.openWritingPipe(addresses[1]);
   radio.openReadingPipe(1,addresses[0]);
   radio.startListening();

  // Initialize motor pins as outputs
  for (int i = 0; i < 2; i++)
  {
    pinMode(inApin[i], OUTPUT);
    pinMode(inBpin[i], OUTPUT);
    pinMode(pwmpin[i], OUTPUT);
  }   
    pinMode(stdbyMotorpin, OUTPUT);
    digitalWrite(stdbyMotorpin, LOW);


    neckServo.attach(neckPwmPin);
    neckServo.write(neck_midpoint); 

 }

  if ( role == role_remote )  { //setup remote pins
   radio.openWritingPipe(addresses[0]);
   radio.openReadingPipe(1,addresses[1]);
   radio.stopListening(); 

   pinMode(AX_pin, INPUT);
   pinMode(AY_pin, INPUT);
   pinMode(BTN_pin, INPUT);
   digitalWrite(BTN_pin, HIGH); 

   pinMode(neck_up_pin, INPUT);
   pinMode(neck_down_pin, INPUT);
   digitalWrite(neck_up_pin, HIGH);  
   digitalWrite(neck_down_pin, HIGH);  
    // Bounce object with a 20 millisecond debounce time
    bouncer.attach(BTN_pin);
    bouncer.interval(20);
  }

  if(DEBUG_PONG)   radio.printDetails();
}

void loop(void)
{
  if ( role == role_remote )
  {

    myData.xout = analogRead(AX_pin);
    myData.yout = analogRead(AY_pin);
    bouncer.update();
    myData.btn = bouncer.read();
    
    if(!digitalRead(neck_down_pin))
      myData.neck = -1;
    else if(!digitalRead(neck_up_pin))
      myData.neck = 1;
    else
      myData.neck = 0;     

    // now Send the state to bot
    bool ok = radio.write(&myData, sizeof(myData));
    
    if(ok)
    Serial<<"transfer OK  \n\r";
    else
    Serial<<"transfer failed \n\r";
  }

  if ( role == role_bot )
  {
    if ( radio.available() )
    {
      while(radio.available()){
        radio.read(&myData, sizeof(myData));
      }

      //wheels motor control
      x2pwm = map(myData.xout, 0, 1024, 255, -255);
      y2pwm = map(myData.yout, 0, 1024, 255, -255);
      int leftMotor = constrain(y2pwm + x2pwm, -255, 255);
      int rightMotor = constrain(y2pwm - x2pwm, -255, 255);

      //controller in rest has this values
      //if (myData.yout == 498 && (myData.xout == 482 || myData.xout == 481) ) {
      if (
        (myData.yout > joystickYmid-joystickNoise && myData.yout < joystickYmid+joystickNoise)
        && (myData.xout > joystickXmid-joystickNoise && myData.xout < joystickXmid+joystickNoise)
        ){
        digitalWrite(stdbyMotorpin, LOW);     //stop board from consuming power
      }else{
        Serial<<myData.xout<<" "<<myData.yout<<endl;
        move(0, leftMotor);
        move(1, rightMotor);
      }
      //////////

      /// neck control
      switch (myData.neck) {
          case -1:
            neckServo.write(neck_midpoint + neck_speed);
            break;
          case 0:
            neckServo.write(neck_midpoint);
            break;
          case 1:
            neckServo.write(neck_midpoint - neck_speed);
            break;
      }
      /////////////////
      if (DEBUG_PONG)
      {
          // Serial
          // <<"X> "<< myData.xout
          // << " |Y> " << myData.yout
          // << " |leftM> " << leftMotor 
          // << " |rightM> " << rightMotor 
          // <<" |BTN> "<< myData.btn
          // <<" |neck> "<< myData.neck
          // << endl;
          Serial <<
          myData.xout
          << " " << 
          myData.yout
          << " " << 
          myData.neck
          << endl;
          delay(4);
        }
      }
    }
  }

// motor functions
void move(int motor, int speed) {
      digitalWrite(stdbyMotorpin, HIGH);
      if (speed < 0)
        {
        digitalWrite(inApin[motor], HIGH);
        digitalWrite(inBpin[motor], LOW); 
        }
      else{ 
        digitalWrite(inApin[motor], LOW);
        digitalWrite(inBpin[motor], HIGH);
      }

      analogWrite(pwmpin[motor], abs(speed));
}


