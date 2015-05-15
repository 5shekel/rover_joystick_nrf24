
/*
  Joystick controlled differential steer mobile robot.
  Author: Dustin Maki
  Date: Sept 1 2011
  GPL License.  Free to use, just give credit where due.
  This program scales the joystick reading to start at 1 just outside the deadband so you do not have a large jump in speed.
  It adjusts to the common case where the centered joystick does not rest at quite the center of the range.
  This was first used with 
  Teensy++ v.1 http://www.pjrc.com/teensy/pinout.html 
  Solarbotics LM298D H bridge http://www.solarbotics.com/products/k_cmd/
  with transistor on each side to switch the direction as in datasheet.
  Joystick http://www.sparkfun.com/products/9032  
     
*/

/*
#define START_BUTTON 21 // not a momentary switch, a toggle switch will work
#define JOYSTICK_BUTTON_PIN 10 // hold joystick with 'tail' toward you.  side with holes on sparkfun breakout board
#define JOYSTICK_X_PIN A0 //full left is 1023, full right is 0
#define JOYSTICK_Y_PIN A1//full forward is 1023, full backward is 0
#define LEFT_PWM_PIN 14
#define LEFT_DIRECTION_PIN 13
#define RIGHT_PWM_PIN 24
#define RIGHT_DIRECTION_PIN 23
#define HALF_DEADBAND 3 //adjust till desired deadband is achieved
#define XRAW_CENTERED 482 // set to whatever "x joystick value is: " on the serial monitor
#define YRAW_CENTERED 498 // set to whatever "y joystick value is: " on the serial monitor
#define DEBUG true // set to false to stop serial messages

void setup()
{ 
  if (DEBUG)              
  Serial.begin(38400);
  
  pinMode(LEFT_PWM_PIN, OUTPUT);
  pinMode(RIGHT_PWM_PIN, OUTPUT);  
  pinMode(LEFT_DIRECTION_PIN, INPUT);// sets high impedance on pin, brake
  pinMode(RIGHT_DIRECTION_PIN, INPUT);// sets high impedance on pin, brake
  digitalWrite(LEFT_PWM_PIN, LOW);  
  digitalWrite(RIGHT_PWM_PIN, LOW);
}

int throttle, steering;
int xraw, yraw, leftSpd, rightSpd;

void loop()                     
{
  // start button is a push on, push off type. grounded when on
  // when start button is released, program just runs main loop
  while(digitalRead(START_BUTTON) == LOW)
  {
    if (digitalRead(JOYSTICK_BUTTON_PIN) == LOW)//brake requested
    {
      pinMode(LEFT_DIRECTION_PIN, INPUT);// sets high impedance on pin, brake
      pinMode(RIGHT_DIRECTION_PIN, INPUT);// sets high impedance on pin, brake
    
      if (DEBUG)
      { 
        Serial.println("brake applied");
      }
      continue;// will not continue main loop, only while
    }
    
  else //start button pressed and not braked 
  {  
    // capture joystick position
    yraw = analogRead(JOYSTICK_Y_PIN);
    xraw = analogRead(JOYSTICK_X_PIN);
    
    // scale x and y axis so that both are zeroed at natural center of potentiometer, implement deadband
    if(yraw > YRAW_CENTERED + HALF_DEADBAND)//forward
    {
      throttle = map(yraw, YRAW_CENTERED + HALF_DEADBAND, 1023, 0, 255);      
      if(xraw > XRAW_CENTERED + HALF_DEADBAND)//left
      {
        steering = map(xraw, XRAW_CENTERED + HALF_DEADBAND, 1023, -0, -255);// reverse pot coordinates to agree with cartesian        
      }
      else if(xraw < XRAW_CENTERED - HALF_DEADBAND)// right
      {
        steering = map(xraw, XRAW_CENTERED - HALF_DEADBAND, 0, 0, 255);// reverse pot coordinates to agree with cartesian        
      }
      else // in X deadband
      {
        steering = 0;
      }
    }
    
    else if(yraw < YRAW_CENTERED - HALF_DEADBAND)//reverse
    {
      throttle = map(yraw, YRAW_CENTERED - HALF_DEADBAND, 0, -0, -255);      
      if(xraw > XRAW_CENTERED + HALF_DEADBAND)//left
      {
        steering = map(xraw, XRAW_CENTERED + HALF_DEADBAND, 1023, -0, -255);// reverse pot coordinates to agree with cartesian        
      }
      else if(xraw < XRAW_CENTERED - HALF_DEADBAND)// right
      {
        steering = map(xraw, XRAW_CENTERED - HALF_DEADBAND, 0, 0, 255);// reverse pot coordinates to agree with cartesian        
      }
      else// in X deadband
      {
        steering = 0;
      }
    }
    else //in Y deadband
    {
      throttle = 0;
      if(xraw > XRAW_CENTERED + HALF_DEADBAND)//left
      {
        steering = map(xraw, XRAW_CENTERED + HALF_DEADBAND, 1023, -0, -255);// reverse pot coordinates to agree with cartesian        
      }
      else if(xraw < XRAW_CENTERED - HALF_DEADBAND)// right
      {
        steering = map(xraw, XRAW_CENTERED - HALF_DEADBAND, 0, 0, 255);// reverse pot coordinates to agree with cartesian        
      }
      else// in X deadband
      {
        steering = 0;
      }
    }
    
  //transfer from x,y to l,r
  leftSpd = throttle + steering;
  rightSpd = throttle - steering;
  //leftSpd and rightSpd will contain a number from -255 to 255 that gets updated each time the loop runs.
  //-255 equates to full reverse. 0 equates to anywhere within the deadband.  255 is full forward.
  
  // The next 2 sections can be replaced to use with servos, stepper motors, BLDCs or whatever drive you like.
  
  //write direction pin
  if (leftSpd < 0)
  digitalWrite(LEFT_DIRECTION_PIN, LOW);
  else if (leftSpd > 0)
  digitalWrite(LEFT_DIRECTION_PIN, HIGH);  
  if (rightSpd < 0)
  digitalWrite(RIGHT_DIRECTION_PIN, LOW);
  else if (rightSpd > 0)
  digitalWrite(RIGHT_DIRECTION_PIN, HIGH);
  
  //write PWM pin
  analogWrite(LEFT_PWM_PIN, leftSpd);
  analogWrite(RIGHT_PWM_PIN, rightSpd);
  
 
  if (DEBUG)
  { 
  Serial.print("x joystick value is: ");
  Serial.println(xraw);
  Serial.print("y joystick value is: ");
  Serial.println(yraw);
  Serial.print("throttle is: ");
  Serial.println(throttle);
  Serial.print("steering is: ");
  Serial.println(steering);
  Serial.print("leftSpd is: ");
  Serial.println(leftSpd);
  Serial.print("rightSpd is: ");
  Serial.println(rightSpd);
  
  delay(250);
  }
 }
 }
}

*/