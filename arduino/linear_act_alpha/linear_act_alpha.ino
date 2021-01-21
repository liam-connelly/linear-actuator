#include <pt.h>   // include protothread library
#include <AccelStepper.h>

#define dirPin 2
#define stepPin 3
#define motorInterfaceType 1

#define stepsPerRev 200
#define microstep 8
#define mmPerRev 5

#define empScale 1.15

long readNumber = 0;

char incomeChar;
String incomeString;

float distancemm = 0;
unsigned long rps = 0;
//unsigned long rpm = 60;
float freq = 1;
int mode = 0;

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {

  Serial.begin(9600);

  while (!Serial) {
    delay(5);
  }

  stepper.setMinPulseWidth(25);
  stepper.setAcceleration(empScale * 1.5 * 16.0 * distancemm * microstep * stepsPerRev * freq * freq * 1.0 / mmPerRev);
  stepper.setMaxSpeed(empScale * 4 * distancemm * microstep * stepsPerRev * freq * 1.0 / mmPerRev);

  pinMode(8,OUTPUT);

}

void loop() {

  if (distancemm == 0 || freq == 0) {
    delay(200);
  }
  
  if (mode == 0) {
        
    stepper.runToNewPosition(distancemm * microstep * stepsPerRev * 1.0 / mmPerRev);
    delay(100);
    stepper.setCurrentPosition(0);

    distancemm = 0;

  } else if (mode == 1 && abs(distancemm) > 0 && freq > 0) {
    
    stepper.runToNewPosition(distancemm * microstep * stepsPerRev * 1.0 / mmPerRev);
    digitalWrite(8,HIGH);
    delay(1);
    digitalWrite(8,LOW);
    stepper.runToNewPosition(0);

  }
  
  if (Serial.available () > 0) {
    incomeString = "";
  } else {
    return;
  }

  while (Serial.available() > 0) {

    stepper.runSpeed();

    incomeChar = ((byte) Serial.read());
    incomeString += incomeChar;

  }

  char copy[incomeString.length()];
  incomeString.toCharArray(copy, incomeString.length());

  char *command = strtok(copy, "&");

  while (command != NULL) {

    char* seperator = strchr(command, '=');

    if (seperator != 0) {

      *seperator = 0;

      ++seperator;
      
      float readNumber = atof(seperator);

      if (!(strcmp("freq", command))) {

        freq = readNumber;

        stepper.setAcceleration(empScale * 1.5 * 16.0 * abs(distancemm) * microstep * stepsPerRev * freq * freq * 1.0 / mmPerRev);
        stepper.setMaxSpeed(empScale * 2.0 * abs(distancemm) * microstep * stepsPerRev * freq * 1.0 / mmPerRev);

      } else if (!(strcmp("dist", command))) {

        distancemm = readNumber;
        
        stepper.setAcceleration(empScale * 1.5 * 16.0 * abs(distancemm) * microstep * stepsPerRev * freq * freq * 1.0 / mmPerRev);
        stepper.setMaxSpeed(empScale * 2.0 * abs(distancemm) * microstep * stepsPerRev * freq * 1.0 / mmPerRev);

      } else if (!(strcmp("mode",command))) {
        
        mode = round(readNumber);
        
      }
    }

    command = strtok(NULL, "&");

  }


}
