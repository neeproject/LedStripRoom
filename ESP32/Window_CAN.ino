#include <Stepper.h>
#include <cstdint>  //Potem usun¹æ, potrzebne tylko do VS, aby podœwietlaæ inty.

const int stepsPerRevolution = 2048;  //kroki silnika

Stepper myStepper = Stepper(stepsPerRevolution, 18, 20, 19, 21);  //Piny silnika 1,3,2,4


void servostop() {
  //if button klikniêty zatrzymaj serwo i ustaw stan na 0, jeœli jest inny.
}

void homeposition() {
  //if button nie klikniêty, zacznij jechaæ w dó³, a¿ siê kliknie. Wtedy samo uruchomie siê servostop().
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

