//LED Lamp
//This is a simple Arduino program using existing
//libraries to run 6 LEDs to run up a small lamp.
//The CapSense, Median Filter and FastLED libraries
//were all chosen for their relative ease of use
//and availability within the Arduino IDE.

#include <CapacitiveSensor.h>
#include <MedianFilter.h>
#include <FastLED.h>

#define LED_PIN 13 //Set LED Data Pin for FastLED
#define NUM_LEDS 6 //Maximum Number of LEDs for FastLED

CRGBArray<NUM_LEDS> leds; //Create an array for FastLED with values for all the LEDs in the chain
const int ledData = 13; //Set Variable ledData to reference LED data pin

MedianFilter senVal(3, 0); //Create a list using median filter. List is 3 values and seed with value '0'
CapacitiveSensor capSensor = CapacitiveSensor(4, 2); //Pin4 is send, pin 2 is receive
                                                     //Pins are bridged with 1Mohm resistor 
                                                     //Pin 2 is connect connected to the end sensor point 
int senThresh = 1750; //Threshold value for Capacitive Sensor. Lets us control when to change Pass to True
                      //A smaller value will trigger the sensor without touching the sensor
                      //1750-2300 works well for direct touch input

//LED Settings
//Each program is stored as a part of this pointer array
void (*ledShow[])() = {
  []{static uint8_t hue=0; leds.fill_rainbow(hue++); FastLED.delay(20);},
  []{for(int i = 0; i < NUM_LEDS; i++){ leds[i] = CRGB::Purple; FastLED.show();}},
  []{for(int i = 0; i < NUM_LEDS; i++){ leds[i] = CRGB::Green; FastLED.show();}},
  []{for(int i = 0; i < NUM_LEDS; i++){leds[i] = CRGB::Yellow; FastLED.show();}},
  []{for(int i = 0; i < NUM_LEDS; i++){leds[i] = CRGB::White;FastLED.show();}}
};

void setup() {
  pinMode(ledData, OUTPUT); // Set the LED Pin mode
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS); //Set up FastLED Device Type, Output Pin, diode arrangement, FastLED array and Number of LEDs

//  Serial.begin(9600); //activate for Debuging. Not Necessary for operation

}

void loop() {

  static int ledState = 0;      //Initialize the LEDState Variable and set it to 0 
  static int ledMax = 4;        //Set the max number of LED states we can run through
                                //I'm sure there's a way to get the max number of entries in
                                //ledShow[], but I don't know that function so hardcode hacks for now
                                
  ledShow[ledState]();          //Initialize LEDs, start on first program in ledShow[]

  long sensorValue = capSensor.capacitiveSensor(20); //Get the value of the capacitive sensor for 20 samples
  senVal.in( sensorValue);                           //Pass the sensor value into the median filter list
  sensorValue = senVal.out();                        //Return the filtered sensor value. Gives a median of the capSense values

//  Serial.println("Sensor Value: ");                //SERIAL MONITOR: check the sensor value
//  Serial.println(sensorValue);                     
  
//Check Sensor for someone touching it and then increment the
//ledState counter to run through functions inside of the ledShow array
  while(sensorValue>=senThresh){
    sensorValue = 0;              //Reset the Sensor to 0
    ledState = ledState + 1;      //Increment LED program and reset if it goes above the total number
    if (ledState > ledMax){
      ledState = 0; 
    }
    ledShow[ledState]();          //display the new LED program

//  Serial.println("LED State: "); //SERIAL MONITOR: check which LED program we're running
//  Serial.println(ledState);
    
    delay(350);       //Wait 350 milliseconds before progressing, likely enough time for a user to remove their finger
                      //Added benefit of allowing users to cycle the LED programs by holding their finger on the sensor
    
    sensorValue = 0;  //No really, reset the sensor value to Zero;
    senVal.in(0);     //tell the MedianFilter we want Zero as the value to restart its counter;
    break;            //Start void loop() over instead of just hanging out inside this while statement
    }
}
