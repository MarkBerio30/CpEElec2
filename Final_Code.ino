#include <EEPROM.h> // Include EEPROM library for working with EEPROM memory
#include <LiquidCrystal_I2C.h> // Include LiquidCrystal_I2C library for LCD display
#include <Wire.h>  // Include the Wire library
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Create an object for LCD display
 
#include <SoftwareSerial.h>  // Include SoftwareSerial library for software serial communication
SoftwareSerial GSM(10, 11); // Create an object for GSM module RX 11 pin and 10 TX pin
 
  #define buzzer 7  // Define the pin number for the buzzer
  #define relay 8  // Define the pin number for the relay
  #define bt_theft A0  // Define the pin number for the theft detection micro limit switch 
  #define pulse_in 2    // Define the pin number for the pulse input Optocoupler IC 4N35
 
  char inchar; // Variable to store incoming character from serial communication
   
  int unt_a = 0, unt_b = 0, unt_c = 0, unt_d = 0; // Variables to store unit values for different appliances
  long total_unt = 7;   // Variable to store total units consumed
  int price = 0;    // Variable to store price per unit
  long price1 = 0; // Variable to store total price
  int Set = 10;  // Variable to set threshold for theft detection
  volatile int pulse = 0; // Variable to store pulse count
  
  String phone1 = "+639613281759";// String to store phone number 1 for sending SMS
  String phone2 = "+639085306405"; // String to store phone number 2 for sending SMS
 
  int flag1 = 0, flag2 = 0, flag3 = 0; // Flags for different conditions

 
void setup() {
  Serial.begin(9600); // Start serial communication at 9600 bps
  GSM.begin(9600);  // Start GSM module communication at 9600 bps
 
  pinMode(bt_theft, INPUT_PULLUP); // Set bt_theft pin as input with internal pull-up resistor
  pinMode(relay, OUTPUT);  // Set relay pin as output
  pinMode(buzzer, OUTPUT); // Set buzzer pin as output
 
  pinMode(pulse_in, INPUT); // Set pulse_in pin as input
  attachInterrupt(digitalPinToInterrupt(pulse_in), ai0, RISING);    // Attach interrupt to pulse_in pin with ai0 function to handle rising edge
 
  lcd.begin(16,2); // Initialize LCD display
  lcd.backlight(); // Turn on backlight
  lcd.clear(); // Clear LCD display
  lcd.setCursor(5, 0); // Set cursor position to (5,0)
  lcd.print("IoT"); // Display "WELCOME" on LCD
  lcd.setCursor(1, 1); // Set cursor position to (2,1)
  lcd.print("Energy Meter"); // Display "IoT Energy Meter" on LCD
 
  Serial.println("Initializing...."); // Print "Initializing...." on serial monitor
  initModule("AT", "OK", 1000); // Initialize GSM module with "AT" command and wait for "OK" response within 1000ms
  initModule("ATE1", "OK", 1000); // Enable echo mode for GSM module with "ATE1" command and wait for "OK" response within 1000ms
  initModule("AT+CPIN?", "READY", 1000); // Check SIM card status with "AT+CPIN?" command and wait for "READY" response within 1000ms
  initModule("AT+CMGF=1", "OK", 1000); // Set SMS text mode with "AT+CMGF=1" command and wait for "OK" response within 1000ms
  initModule("AT+CNMI=2,2,0,0,0", "OK", 1000); // Configure SMS message format with "AT+CNMI=2,2,0,0,0" command and wait for "OK" response within 1000ms
  Serial.println("Initialized Successfully"); // Print "Initialized Successfully" on serial monitor
  delay(100);
  
  sendSMS(phone1, "Hello Mr. Berio Welcome To GSM Based Prepaid Electricity Meter with Theft Detection using Arduino ATMEGA"); // Send SMS to phone1 with "Hello Mr. Berio Welcome To GSM Based Prepaid Electricity Meter with Theft Detection using Arduino ATMEGA" as message
  sendSMS(phone2, "Hello Mr. Tomakin Welcome To GSM Based Prepaid Electricity Meter with Theft Detection using Arduino ATMEGA"); // Send SMS to phone2 with "Mr. Tomakin Welcome To GSM Based Prepaid Electricity Meter with Theft Detection using Arduino ATMEGA" as message

  lcd.clear(); // Clear LCD display
 
  if (EEPROM.read(50) == 0) {}  // Check if EEPROM value at address 50 is 0
  else {
    Write(); // Call Write() function to write initial values to EEPROM
  }
  EEPROM.write(50, 0);   // Write 0 to EEPROM address 50
  pulse = EEPROM.read(10); // Read value from EEPROM address 10 and store in pulse variable
  Read();  // Call Read() function to read values from EEPROM
  if (total_unt > 0) {
    digitalWrite(relay, HIGH); // Turn on relay if total_unt is greater than 0
  }
}

void loop()
{
 
 if (GSM.available() > 0) { // Check if there is any data available from GSM module
    inchar = GSM.read(); // Read a character from GSM module and store it in 'inchar' variable
    if ((inchar == 'E') || inchar == 'e' ) { // Check if the character is 'E' and 'e'
      delay(10);
      inchar = GSM.read();
      if ((inchar == 'N')|| (inchar == 'n')) { // Check if the next character is 'n' and 'N'
        delay(10);
        inchar = GSM.read();
        if ((inchar == 'E')|| (inchar == 'e')){ // Check if the next character is 'e' and 'E'
          delay(10);
          inchar = GSM.read();
          if ((inchar == 'R') || (inchar == 'r')){ // Check if the next character is 'r' and 'R'
            delay(10);
            inchar = GSM.read();
            if ((inchar == 'G')|| (inchar == 'g')) { // Check if the next character is 'g' and 'G'
              delay(10);
              inchar = GSM.read();
              if ((inchar == 'Y') || (inchar == 'y')) { // Check if the next character is 'y' and 'Y'
                delay(10);
                inchar = GSM.read();
                if (inchar == '1') { // Check if the next character is '1'
                  price = 200 / Set;  // Calculate price based on a predefined variable 'Set'
                  total_unt = total_unt + price; // Update total unit consumed
                  sendSMS(phone1, "Your recharge has been successfully completed with 200 units"); // Send SMS with a message
                  load_on(); // Call a function to turn on a load
                }
                else if (inchar == '2') { // Check if the next character is '2'
                  price = 400 / Set;  // Calculate price based on a predefined variable 'Set'
                  total_unt = total_unt + price; // Update total unit consumed
                  sendSMS(phone1, "Your recharge has been successfully completed with 400 units"); // Send SMS with a message
                  load_on(); // Call a function to turn on a load
                }
                else if (inchar == '3') { // Check if the next character is '3'
                  price = 700 / Set;  // Calculate price based on a predefined variable 'Set'
                  total_unt = total_unt + price; // Update total unit consumed
                  sendSMS(phone1, "Your recharge has been successfully completed with 700 units"); // Send SMS with a message
                  load_on(); // Call a function to turn on a load
                }
                else if (inchar == '4') { // Check if the next character is '4'
                  price = 900 / Set;  // Calculate price based on a predefined variable 'Set'
                  total_unt = total_unt + price; // Update total unit consumed
                  sendSMS(phone1, "Your recharge has been successfully completed with 900 units"); // Send SMS with a message
                  load_on(); // Call a function to turn on a load
                }
               
                delay(10);
              }
            }
          }
        }
      }
    }
 
    else if ((inchar == 'D') || (inchar == 'd')) { // Check if the character is 'D' and 'd'
        delay(10);
        inchar = GSM.read();
    if ((inchar == 'A') || (inchar == 'a')) { // Check if the next character is 'A' and 'a'
        delay(10);
        inchar = GSM.read();
    if ((inchar == 'T') || (inchar == 't')) { // Check if the next character is 'T' and 't'
        delay(10);
        inchar = GSM.read();
    if ((inchar == 'A') || (inchar == 'a')) { // Check if the next character is 'A' and 'a'
        Data(); // Call a function named 'Data()'
    }
   }
  }
 }
}
 
  lcd.setCursor(0, 0); // Set the cursor of the LCD to position (0, 0) - first row, first column
  lcd.print("Unit: "); // Print "Unit:" on the LCD
  lcd.print(total_unt); // Print the value of 'total_unt' variable on the LCD
  lcd.print("   "); // Print three spaces on the LCD
   
  lcd.setCursor(0, 1); // Set the cursor of the LCD to position (0, 1) - second row, first column
  lcd.print("Price: "); // Print "Price:" on the LCD
  lcd.print(price1); // Print the value of 'price1' variable on the LCD
  lcd.print("   "); // Print three spaces on the LCD
   
  lcd.setCursor(11, 0); // Set the cursor of the LCD to position (11, 0) - first row, twelfth column
  lcd.print("Pulse"); // Print "Pulse" on the LCD
   
  lcd.setCursor(13, 1); // Set the cursor of the LCD to position (13, 1) - second row, fourteenth column
  lcd.print(pulse); // Print the value of 'pulse' variable on the LCD
  lcd.print("   "); // Print three spaces on the LCD

  if (total_unt == 5) // Check if the value of 'total_unt' variable is equal to 5
  {
  if (flag1 == 0) // Check if the value of 'flag1' variable is equal to 0
  {
    flag1 = 1; // Set the value of 'flag1' variable to 1
    sendSMS(phone1,  "Your balance is running low. Please recharge."); // Send an SMS with the specified message to the phone number stored in 'phone1' variable
    sendSMS(phone2, "Your balance is running low. Please recharge."); // Send an SMS with the specified message to the phone number stored in 'phone2' variable
   
  }
  } 

 
  if (total_unt == 0) // Check if the value of 'total_unt' variable is equal to 0
  {
    digitalWrite(relay, LOW); // Turn off the relay by setting the corresponding pin to LOW
  if (flag2 == 0) // Check if the value of 'flag2' variable is equal to 0
  {
    flag2 = 1; // Set the value of 'flag2' variable to 1
    sendSMS(phone1, "Your balance has been depleted. Please recharge."); // Send an SMS with the specified message to the phone number stored in 'phone1' variable
    sendSMS(phone2, "Your balance has been depleted. Please recharge."); // Send an SMS with the specified message to the phone number stored in 'phone2' variable
    
  }
  }

 
  if (digitalRead(bt_theft) == 0) // Check if the value of the digital pin connected to 'bt_theft' is LOW (indicating the button is pressed)
  {
  if (flag3 == 0) // Check if the value of 'flag3' variable is equal to 0
  {
    flag3 = 1; // Set the value of 'flag3' variable to 1
    sendSMS(phone1,  "Electricity Meter Theft Detected! Activating Alarm System "); // Send an SMS with the specified message to the phone number stored in 'phone1' variable
    sendSMS(phone2, "Electricity Meter Theft Detected! Activating Alarm System "); // Send an SMS with the specified message to the phone number stored in 'phone2' variable
    digitalWrite(buzzer, HIGH); // Turn off the buzzer by setting the corresponding pin to HIGH
  }
  }
  else
  {
    flag3 = 0; // If the button is not pressed, set the value of 'flag3' variable to 0
     digitalWrite(buzzer, LOW); // Turn off the buzzer by setting the corresponding pin to HIGH
  }

    delay(5); // Introduce a delay of 5 milliseconds

}
 
void load_on()
{
    Write(); // Calls a function named "Write()" - presumably writes data to some device or performs a specific action.
    Read(); // Calls a function named "Read()" - presumably reads data from some device or performs a specific action.
    digitalWrite(relay, HIGH); // Sets the digital pin named "relay" to a HIGH state, typically used to turn ON a relay or any other digital output device.
    flag1 = 0, flag2 = 0; // Sets the values of two variables, "flag1" and "flag2", to 0. This may be resetting the flags to an initial state for further use in the code.
}

 
void sendSMS(String number, String msg)
{
    GSM.print("AT+CMGS=\""); // Sends the command "AT+CMGS=" to the GSM module, which is a standard command used to start an SMS message sending process.
    GSM.print(number); // Sends the phone number (destination) for the SMS message.
    GSM.println("\"\r\n"); // Sends a closing quotation mark, carriage return, and line feed to terminate the phone number command.
    delay(500); // Adds a delay of 500 milliseconds (half a second) to allow for the GSM module to process the command.
    GSM.println(msg); // Sends the actual content of the SMS message.
    delay(500); // Adds another delay of 500 milliseconds.
    GSM.write(byte(26)); // Sends an ASCII control character 26 (substitute) to indicate the end of the SMS message and initiate the sending process.
    delay(5000); // Adds a longer delay of 5000 milliseconds (5 seconds) to allow for the SMS message to be sent and processed by the GSM module.
}

 
void Data()
{
    GSM.print("AT+CMGS=\""); // Sends the command "AT+CMGS=" to the GSM module, which is a standard command used to start an SMS message sending process.
    GSM.print(phone1); // Sends the phone number (destination) for the SMS message, presumably stored in a variable named "phone_no1".
    GSM.println("\"\r\n"); // Sends a closing quotation mark, carriage return, and line feed to terminate the phone number command.
    delay(1000); // Adds a delay of 1000 milliseconds (1 second) to allow for the GSM module to process the command.
    GSM.print("Unit:"); // Sends the string "Unit:" to represent the data being sent, possibly indicating the measurement unit.
    GSM.println(total_unt); // Sends the value of a variable named "total_unt" to represent the actual data being sent.
    GSM.print("Price:"); // Sends the string "Price:" to represent the data being sent, possibly indicating the price associated with the data.
    GSM.println(price1); // Sends the value of a variable named "price1" to represent the actual data being sent.
    delay(500); // Adds a delay of 500 milliseconds (half a second).
    GSM.write(byte(26)); // Sends an ASCII control character 26 (substitute) to indicate the end of the SMS message and initiate the sending process.
    delay(5000); // Adds a longer delay of 5000 milliseconds (5 seconds) to allow for the SMS message to be sent and processed by the GSM module.
}

 
void Read()
{
    unt_a = EEPROM.read(1); // Reads a byte of data from EEPROM address 1 and stores it in a variable named "unt_a".
    unt_b = EEPROM.read(2); // Reads a byte of data from EEPROM address 2 and stores it in a variable named "unt_b".
    unt_c = EEPROM.read(3); // Reads a byte of data from EEPROM address 3 and stores it in a variable named "unt_c".
    unt_d = EEPROM.read(4); // Reads a byte of data from EEPROM address 4 and stores it in a variable named "unt_d".
  
    // Combines the retrieved bytes of data to form a single integer value representing the total units.
    total_unt = unt_d * 1000 + unt_c * 100 + unt_b * 10 + unt_a;
  
    // Performs a calculation to determine the price based on the total units and a variable named "Set".
    price1 = total_unt * Set;
}

 
void Write()
{
    // Calculates the individual digits of the total units value and stores them in respective variables.
    unt_d = total_unt / 1000; // Calculates the thousands digit.
    total_unt = total_unt - (unt_d * 1000); // Updates total_unt to exclude the thousands digit.
    unt_c = total_unt / 100; // Calculates the hundreds digit.
    total_unt = total_unt - (unt_c * 100); // Updates total_unt to exclude the hundreds digit.
    unt_b = total_unt / 10; // Calculates the tens digit.
    unt_a = total_unt - (unt_b * 10); // Calculates the ones digit.
  
    // Writes the calculated digits to respective EEPROM addresses.
    EEPROM.write(1, unt_a); // Writes the ones digit to EEPROM address 1.
    EEPROM.write(2, unt_b); // Writes the tens digit to EEPROM address 2.
    EEPROM.write(3, unt_c); // Writes the hundreds digit to EEPROM address 3.
    EEPROM.write(4, unt_d); // Writes the thousands digit to EEPROM address 4.
}

 
 
void initModule(String cmd, char *res, int t)
{
    while (1)
    {
      // Sends the command to the GSM module over Serial and GSM interfaces.
      Serial.println(cmd); // Prints the command to the Serial monitor for debugging purposes.
      GSM.println(cmd); // Sends the command to the GSM module.
  
      delay(100); // Waits for a short delay.
  
      // Checks for a response from the GSM module.
      while (GSM.available() > 0) // Checks if there is any data available from the GSM module.
      {
        if (GSM.find(res)) // Searches for the expected response in the data received from the GSM module.
        {
          Serial.println(res); // Prints the expected response to the Serial monitor for debugging purposes.
          delay(t); // Waits for a specified delay time.
          return; // Returns from the function after receiving the expected response.
        }
        else
        {
          Serial.println("Error"); // Prints an error message to the Serial monitor if the expected response is not found.
        }
      }
  
      delay(t); // Waits for a specified delay time before repeating the loop.
    }
}

 
void ai0()
{
    if (digitalRead(pulse_in) == 1) // Checks if the pulse input signal is high (1).
    {
      pulse++; // Increments the pulse count by 1.
  
      if (pulse > 9) // Checks if the pulse count exceeds 9.
      {
        pulse = 0; // Resets the pulse count to 0.
        if (total_unt > 0) // Checks if the total units is greater than 0.
        {
          total_unt = total_unt - 1; // Decrements the total units by 1.
        }
        Write(); // Writes the updated total units to EEPROM.
        Read(); // Reads the updated total units and price from EEPROM.
      }
  
      EEPROM.write(10, pulse); // Writes the pulse count to EEPROM at address 10.
    }
}
