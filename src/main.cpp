/******************************************************************************
 * @file main.cpp
 *
 * @mainpage Hexbot firmware. 
 * 
 * @section intro_sec Introduction
 *
 * This code is the firmware for the six legged robot called Hexbot. Full
 * details on how to get the circuit and chassis for this robot are found
 * [here](https://github.com/va3wam/hexBot). 
 *
 * @section dependencies Dependencies
 * 
 * This sketch depends on the libraries located in the main.h file located in
 * the include directory of this repository.
 *
 * @section author Author(s)
 *
 * Written by Old Squire and Doug Elliott.
 *
 * @section license license
 *
 * Copyright 2021 the Aging Apprentice 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to 
 * deal in the Software without restriction, including without limitation the 
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions: The above copyright
 * notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.  
 *****************************************************************************/
#include <main.h> // Header file for all libraries needed by this program.
unsigned long timer; // Milli count for next action.
int32_t cadencePeriod = 2000; // How long between phases in millis.

/**
 * @brief Standard Arduino initialization routine.
 * ==========================================================================*/
void setup() 
{
   setupSerial(); // Set serial baud rate. 
   Serial.println("<setup> Start of setup.");  
   Wire.begin(I2C_bus0_SDA, I2C_bus0_SCL, I2C_bus0_speed); // Init I2C bus0.
   network.connect(); // Start WiFi connection.
   if(network.areWeConnected() == true) // If we are on the WiFi network.
   {
      networkConnected = true;
      Serial.println("<setup> Connection to network successfully estabished.");
      startWebServer(); // Start up web server.
      bool tmp = connectToMqttBroker(network); // Connect to MQTT broker.
      if(tmp == true) // If we found an MQTT broker.
      {
         mqttBrokerConnected = true;
         Serial.println("<setup> Connection to MQTT broker successfully established.");
      } // if
      else // If we did not find a valid MQTT broker.
      {
         mqttBrokerConnected = false;
         Serial.println("<setup> Connected to MQTT broker failed.");
      } // else
   } // if
   else // If we are NOT on the WiFi network.
   {
      networkConnected = false;
      Serial.println("<setup> Not connencted to the network. No MQTT or web interface.");
   } // else
   scanBus0(); // Scan bus0 and show connected devices.
   if(oledConnected == true) // If an OLED was found on the I2C bus.
   {
      Serial.println("<setup> Initialize OLED.");
      initOled();
   } // if
   else // If an OLED was NOT found on the I2C bus.
   {
      Serial.println("<setup> OLED not connencted to I2C bus. No OLED messages to be issued.");
   } //else
   if(motorController1Connected == true && motorController2Connected == true) // If servo drivers found on I2C bus.
   {
      Serial.println("<setup> Initialize servo drivers.");
      initServo();   
   } // if
   else // If servo drivers found on I2C bus.
   {
      Serial.println("<setup> One or more servo drivers not connencted to I2C bus. No motion is possible.");
   } //else
   showCfgDetails(); // Show all configuration details in one summary.
   timer = millis(); // Timer for motor driver signalling.
   Serial.println("<setup> End of setup."); 
} // setup()

/**
 * @brief Standard Arduino main loop.
 * ==========================================================================*/
void loop() 
{
   monitorWebServer(); // Handle any pending web client requests. 
   if(timer <= millis()) // Time to update motor position?
   {
//      updatePhase(); // Move motor joints through different positions.
      timer = millis() + cadencePeriod; // Set next motor driver update time.
   } //if
   checkOledButtons();
} // loop()  