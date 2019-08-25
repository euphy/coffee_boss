/**
 *
 * HX711 library for Arduino - example file
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/
#include "HX711.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 21;
const int LOADCELL_SCK_PIN = 22;


HX711 scale;

void setup() {
  Serial.begin(57600);
  Serial.println("HX711 Demo");

  Serial.println("Initializing the scale");

  // Initialize library with data output pin, clock input pin and gain factor.
  // Channel selection is made by passing the appropriate gain:
  // - With a gain factor of 64 or 128, channel A is selected
  // - With a gain factor of 32, channel B is selected
  // By omitting the gain factor parameter, the library
  // default "128" (Channel A) is used here.
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);


  scale.set_scale();
  
  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());			// print a raw reading from the ADC

  float rawWithoutWeight = scale.read_average(20);
  Serial.print("read average: \t\t");
  Serial.println(rawWithoutWeight);  	// print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);	// print the average of 5 readings from the ADC minus tare weight (not set) divided
						// by the SCALE parameter (not set yet)

//  scale.tare();
  delay(1000);
  Serial.println("Put a 1kg weight on.");
  delay(5000);
  Serial.println("Continuing...");

  float rawWith1kg = scale.read_average(20);
  Serial.print("read average with 1kg: \t\t");
  Serial.println(rawWith1kg);

  float raw1kgOnly = rawWith1kg - rawWithoutWeight;
  Serial.print("1kg equal to: \t\t");
  Serial.println(raw1kgOnly);

  float scaleValue = raw1kgOnly / 1000.0;
  Serial.print("Setting scale to ");
  Serial.println(scaleValue);
  scale.set_scale(scaleValue);

//  scale.set_scale(2280.f);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  Serial.println("Take the weight off again");
  delay(5000);
  Serial.println("Continuing...");

  
  scale.tare();				        // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
						// by the SCALE parameter set with set_scale

  Serial.println("Readings:");
}

void loop() {
  Serial.print("one reading:\t");
  Serial.print(scale.get_units(), 1);
  Serial.print("\t| average:\t");
  Serial.println(scale.get_units(10), 1);

  scale.power_down();			        // put the ADC in sleep mode
  delay(5000);
  scale.power_up();
}
