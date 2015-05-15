/**
* control remote bot (based on  RF24 lib Example "http://maniacbug.github.io/RF24/led_remote_8pde-example.html")
* This is an example of how to use the RF24 class to control a 2 DC motor bot
* https://github.com/5shekel/rover_joystick_nrf24
*/


#include <SPI.h>
#include "RF24.h" //https://github.com/maniacbug/RF24
#include "printf.h"
#include <Bounce2.h> // https://github.com/thomasfredericks/Bounce-Arduino-Wiring

#include <MotorDC.h>

MotorDC  m1(6, 8, 13) ;
MotorDC  m2(5, 7, 2, false);

#define DEBUG_PONG 0
RF24 radio(9, 8);
const int role_pin = 4;// sets the role of this unit in hardware.  Connect to GND to be the 'led' board receiver

#define BTN_pin 7
#define AX_pin A0
#define AY_pin A1

uint8_t snsVal[5]; //int>byte array (hi and low for anlog and one byte for btn) AX,AY,btn;
const uint8_t size_snsVal = sizeof(snsVal);

//motor pins
#define MR_pin_0 10
#define MR_pin_1 3
#define ML_pin_0 6
#define ML_pin_1 5

int drv_pins[] = {
  MR_pin_0, MR_pin_1, ML_pin_0, ML_pin_1
};
#define num_drv_pins 4
int xout, yout, btnout;
int x2pwm, y2pwm ;
boolean Xon, Yon; //is the joystick moving

Bounce bouncer = Bounce();

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;

// The various roles supported by this sketch
typedef enum { role_remote = 1, role_bot } role_e;

// The debug-friendly names of those roles
const char* role_friendly_name[] = { "invalid", "Remote", "driver"};

// The role of the current running sketch
role_e role;


void setup(void)
{
  //
  // Role
  //

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
  printf_begin();
  printf("\n\rYAWCB/\n\r");
  printf("ROLE: %s\n\r", role_friendly_name[role]);

  radio.begin();

    // This simple sketch opens a single pipes for these two nodes to communicate
  // back and forth.  One listens on it, the other talks to it.

  if ( role == role_remote )
  {
    radio.openWritingPipe(pipe);
  }
  else
  {
    radio.openReadingPipe(1, pipe);
  }

  if ( role == role_bot )   // Start listening
    radio.startListening();

  radio.printDetails();// Dump the configuration of the rf unit for debugging

  if ( role == role_remote )  { //setup remote pins
    pinMode(AX_pin, INPUT);
    pinMode(AY_pin, INPUT);
    pinMode(BTN_pin, INPUT);
    digitalWrite(BTN_pin, HIGH);  }

  // Turn DRV's off until we start getting data
  if ( role == role_bot )  {
    for (int i = 0; i < 4; i++)
    {
      //printf("i: %i\n\r",i);
      pinMode(drv_pins[i], OUTPUT);
      digitalWrite(drv_pins[i], LOW);
    }
  }

  // Bounce object with a 20 millisecond debounce time
  bouncer.attach(BTN_pin);
  bouncer.interval(20);

}

void loop(void)
{


  if ( role == role_remote )
  {
    bouncer.update();

    //read sensor values
    snsVal[0] = lowByte(analogRead(AX_pin));
    snsVal[1] =  highByte(analogRead(AX_pin));

    snsVal[2] = lowByte(analogRead(AY_pin));
    snsVal[3] =  highByte(analogRead(AY_pin));;

    snsVal[4] = bouncer.read();

    // now Send the state to bot
    printf("Now sending... ");

    bool ok = radio.write( snsVal, 5 );
    if (ok) printf("ok\n\r"); else printf("failed\n\r");

    // Try again in a short while
    delay(20);
  }

  //
  // bot role.
  //

  if ( role == role_bot )
  {

    // if there is data ready
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( snsVal, size_snsVal );


        xout = word(snsVal[1], snsVal[0]);
        yout = word(snsVal[3], snsVal[2]);
        btnout = word(snsVal[4]);

        if (yout < 495) {
          y2pwm = map(yout, 495, 0, 0,  100);

          m1.setSpeed(y2pwm);
          m2.setSpeed(y2pwm);
        }
        else if (yout > 502) {
          y2pwm = map(yout, 502, 1024, 0,  100);

          m1.setSpeed(-y2pwm);
          m2.setSpeed(-y2pwm);
        }
        else {
          Mcontrol('R', 0, 0);
        }

        if (DEBUG_PONG)
        {
          // Spew it
          printf("X %i ", xout); //convert from hi/low to int
          printf("Y %i  ", yout);
          printf("x2pwm %i ", x2pwm);
          printf("y2pwm %i ", y2pwm);
          printf("BTN %i \n\r", btnout);
          delay(4);
        }

      }

    }
  }


}


