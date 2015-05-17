/**
* of how to use the RF24 class to control a 2 DC motor bot
* https://github.com/5shekel/rover_joystick_nrf24

* control remote bot 
* based on  RF24 lib Example http://tmrh20.github.io/RF24/pingpair_dyn_8ino-example.html 
* for clrifing the array sending see http://forum.arduino.cc/index.php?topic=260853.0
*/

#define DEBUG_PONG 1

#include "RF24.h" //http://tmrh20.github.io/ this is a manitned fork, the original is now radiohead
#include <SPI.h>
RF24 radio(9, 8);
const int role_pin = 4;// sets the role of this unit in hardware.  
// The various roles supported by this sketch
typedef enum { role_remote = 1, role_bot } role_e;
// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Remote", "Bot"};
role_e role;// The role of the current running sketch
byte addresses[][6] = {"1Node","2Node"};

#include <Bounce2.h> // https://github.com/thomasfredericks/Bounce-Arduino-Wiring
#include "Streaming.h"

#include "Motor.h" //http://sourceforge.net/projects/drv8833lib
//motor pins
Motor  m1(6, 5); //B1=6 B2=5 
Motor  m2(10, 3); //A1=10 A2=3

#define BTN_pin 7
#define AX_pin A0
#define AY_pin A1

uint8_t snsVal[] = {0,0,0,0,0}; //int>byte array (hi and low for anlog and one byte for btn) AX,AY,btn;
int xout, yout, btnout;
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
  }

  if ( role == role_remote )  { //setup remote pins
   radio.openWritingPipe(addresses[0]);
   radio.openReadingPipe(1,addresses[1]);
   radio.stopListening(); 

    pinMode(AX_pin, INPUT);
    pinMode(AY_pin, INPUT);
    pinMode(BTN_pin, INPUT);
    digitalWrite(BTN_pin, HIGH);  
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
    //read sensor values
    bouncer.update();
    snsVal[0] = lowByte(analogRead(AX_pin));
    snsVal[1] =  highByte(analogRead(AX_pin));
    snsVal[2] = lowByte(analogRead(AY_pin));
    snsVal[3] =  highByte(analogRead(AY_pin));;
    snsVal[4] = bouncer.read();

    // now Send the state to bot
    bool ok = radio.write(&snsVal, sizeof(snsVal));
    
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
        radio.read(&snsVal, sizeof(snsVal));
      }
      xout = word(snsVal[1], snsVal[0]);
      yout = word(snsVal[3], snsVal[2]);
      btnout = word(snsVal[4]);

        if (yout > 495 && yout < 502) {
          m1.stopMotor();
          m2.stopMotor();
          y2pwm = 0;
        }else{
          y2pwm = map(yout, 0, 1024, -100, 100);
          x2pwm = map(xout, 0, 1024, -100, 100);

          m1.setMotorSpeed(y2pwm - x2pwm);
          m2.setMotorSpeed(y2pwm - x2pwm);
          m1.startMotor();
          m2.startMotor();
        }

      if (DEBUG_PONG)
        {
          // Spew it
          Serial<<"X> "<< xout<< " |Y> " << yout<< " |x2pwm> "<< x2pwm << " |y2pwm> " << y2pwm <<" |BTN> "<< btnout<< endl;
          delay(4);
        }
    }



      


  }
}

