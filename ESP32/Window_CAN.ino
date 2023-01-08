#include <Stepper.h>
#include <cstdint>  //Potem usun��, potrzebne tylko do VS, aby pod�wietla� inty.

const int stepsPerRevolution = 2048;  //kroki silnika

Stepper myStepper = Stepper(stepsPerRevolution, 18, 20, 19, 21);  //Piny silnika 1,3,2,4


void servostop() {
  //if button klikni�ty zatrzymaj serwo i ustaw stan na 0, je�li jest inny.
}

void homeposition() {
  //if button nie klikni�ty, zacznij jecha� w d�, a� si� kliknie. Wtedy samo uruchomie si� servostop().
}

void setup() {
  // xd
}

void loop() {

  myStepper.setSpeed(10);
  for (uint8_t i = 0; i < 15; i++) {
    myStepper.step(stepsPerRevolution);
  }
  delay(500);

  myStepper.setSpeed(10);
  for (uint8_t i = 0; i < 15; i++) {
    myStepper.step(-stepsPerRevolution);
  }
  delay(500);
}

