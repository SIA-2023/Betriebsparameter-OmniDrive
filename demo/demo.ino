#include "OmniDriver.h"

OmniDriver driver;

void setup(){
  driver.distance_drive(200 /* Distance in cm */, 0 /* Fahrtrichtung in Grad */);

  delay(2000);

  driver.rotate_left(70 /* Speed in % */);

  delay(2000);

  driver.stop();
  delay(500);
  driver.rotate_right(70 /* Speed in % */);

  delay(2000);
  driver.stop();

  delay(500);

  driver.rotate_degree(30); // Rotieren nach Gradangabe (auch mit negativen Grad möglich)

  delay(500);

  driver.drive(180 /* Fahrtrichtung in Grad */, 80 /* Speed in % */);

  delay(1000);

  driver.stop();
}

void loop(){
  delay(100);
}
