#include <TM1637.h>
#include <BfButton.h>
#include <Servo.h>

int CLK = 6;
int DIO = 7;

int encoderPin1 = 3;
int encoderPin2 = 4;
int buttonPin = 5;

int servoPin = 2;

Servo servoMotor;  

TM1637 tm(CLK,DIO);

int aState;
int aLastState;  
int timer = 0;

bool isLocked = false;

BfButton btn(BfButton::STANDALONE_DIGITAL, buttonPin, true, LOW);

void lockClose(void) {
  //Sends a signal for the servo to go to the 180 degrees position
  servoMotor.write(90);
  isLocked = true;
  delay(1000);
  }

void lockOpen(void) { 
  //Sends a signal for the servo to go to the 0 degrees position
  servoMotor.write(180);
  isLocked = false;
  delay(1000);
  }

//Button press hanlding function
void pressHandler (BfButton *btn, BfButton::press_pattern_t pattern) {
  if (!isLocked) {
  switch (pattern) {
    case BfButton::SINGLE_PRESS:
      lockClose();
      break;
      
    case BfButton::DOUBLE_PRESS:
      lockOpen();
      break;
      
    case BfButton::LONG_PRESS:
      timer = 0;
      break;
  }}
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  tm.init();

  //set brightness; 0-7
  tm.set(3);

  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);

  aLastState = digitalRead(encoderPin1);

  //Attaches the pin to the servo object
  servoMotor.attach(servoPin);

  lockOpen();
  
  //Button settings
  btn.onPress(pressHandler)
  .onDoublePress(pressHandler) // default timeout
  .onPressFor(pressHandler, 1000); // custom timeout for 1 second
  
}


void loop() {
  // put your main code here, to run repeatedly:

  //Wait for button press to execute commands
  btn.read();

  if (!isLocked) {
  aState = digitalRead(encoderPin1);

  //Encoder rotation tracking
  if (aState != aLastState) {     
     if (digitalRead(encoderPin2) != aState) { 
       timer = timer + 10;
     }
     else {
       timer = timer - 10;
     }
     if (timer >= 9990 ) {
       timer = 9990;
     }
     if (timer <= 0 ) {
       timer = 0;
     } 
  }   
  aLastState = aState;
  } else {
      if (timer > 0 ) {
        timer--;
        delay(1000);
      }
      else { 
          lockOpen();
        }
    }

    displayNumber(timer);
   
} 

void displayNumber(int num){   
    tm.display(3, num % 10);   
    tm.display(2, num / 10 % 10);   
    tm.display(1, num / 100 % 10);   
    tm.display(0, num / 1000 % 10);
}
