/**
* control remote bot (based on  RF24 lib Example "LED Remote")
*
* This is an example of how to use the RF24 class to control a remote
* bank of LED's using buttons on a remote control.
* yair99@gmail.com
*
*/


#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h" //https://github.com/maniacbug/RF24
#include "printf.h"
#include <Bounce2.h> // https://github.com/thomasfredericks/Bounce-Arduino-Wiring

#include <MotorDC.h>

MotorDC  m1(6, 8, 13) ;
MotorDC  m2(5, 7, 2, false);

#define DEBUG_PONG 0
//
// Hardware configuration
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 8

RF24 radio(9, 8);


// sets the role of this unit in hardware.  Connect to GND to be the 'led' board receiver
// Leave open to be the 'remote' transmitter
const int role_pin = 4;


//ping will transmit the data from A0,A1 and btn
#define BTN_pin 7
#define AX_pin A0
#define AY_pin A1

uint8_t snsVal[5]; //int>byte array (hi and low for anlog and one byte for btn) AX,AY,btn;

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
//
// Topology
//

// Single radio pipe address for the 2 nodes to communicate.
const uint64_t pipe = 0xE8E8F0F0E1LL;

//
// Role management
//
// Set up role.  This sketch uses the same software for all the nodes in this
// system.  Doing so greatly simplifies testing.  The hardware itself specifies
// which node it is.
//
// This is done through the role_pin
//

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
  //if ( digitalRead(role_pin) )
  //role = role_remote;
  //else
  role = role_bot;

  //
  // Print preamble
  //

  Serial.begin(57600);
  printf_begin();
  printf("\n\rYAWCB/\n\r");
  printf("ROLE: %s\n\r", role_friendly_name[role]);

  //
  // Setup and configure rf radio
  //

  radio.begin();

  //
  // Open pipes to other nodes for communication
  //

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

  //
  // Start listening
  //

  if ( role == role_bot )
    radio.startListening();



  radio.printDetails();// Dump the configuration of the rf unit for debugging

  if ( role == role_remote )
  {

    pinMode(AX_pin, INPUT);
    pinMode(AY_pin, INPUT);
    pinMode(BTN_pin, INPUT);
    digitalWrite(BTN_pin, HIGH);

  }

  // Turn DRV's off until we start getting data
  if ( role == role_bot )
  {

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

//
// Loop
//

void loop(void)
{


  if ( role == role_remote )
  {
    // Update the debouncer
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
        done = radio.read( snsVal, 5 );

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

        /*
        if(yout < 495){
        x2pwm = map(xout, 480, 0, 255,  50);
        y2pwm = map(yout, 495, 0, 255,  50);
        Mcontrol('F', y2pwm, x2pwm);
        }
        else if(yout>502){
        x2pwm = map(xout, 485, 1024, 50,  255);
        y2pwm = map(yout, 502, 1024, 50,  255);
        Mcontrol('B', y2pwm, x2pwm);}
        else{
        Mcontrol('R',0, 0);
        }
        /*
        //dir control
        /*
        if(!Yon){
        if(xout<480){
        Xon=1;
        x2pwm = map(xout, 480, 0, 50,  255);
        Mdir('L', x2pwm);
        }
        else if(xout>485){
        Xon=1;
        x2pwm = map(xout, 485, 1024, 50,  255);
        Mdir('R', x2pwm);
        }
        else{
        Xon=0;
        Mdir('H',0);
        }
        }

        //speed control
        if(!Xon)
        {
        if(yout < 495){
        Yon=1;
        x2pwm = map(xout, 480, 0, 50,  255);
        y2pwm = map(yout, 495, 0, 50,  255);
        Mspeed('F', y2pwm);
        }
        else if(yout>502){
        Yon=1;
        y2pwm = map(yout, 502, 1024, 50,  255);
        Mspeed('B', y2pwm);}
        else{
        Yon=0;
        Mspeed('R',0);
        }
        }
        */
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


