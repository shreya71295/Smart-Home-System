#include <Keypad.h>
#include <EEPROM.h>  // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>      // RC522 Module uses SPI protocol
#include <MFRC522.h>   // Library for Mifare RC522 Devices
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2); 



/* For visualizing whats going on hardware
 * we need some leds and
 * to control door lock a relay and a wipe button
 * (or some other hardware)
 * Used common anode led,digitalWriting HIGH turns OFF led
 * Mind that if you are going to use common cathode led or
 * just seperate leds, simply comment out #define COMMON_ANODE,
 */


#define LED_ON HIGH
#define LED_OFF LOW


#define redLed A3
#define greenLed A2
#define blueLed 99
#define relay 92
#define wipeB 3 // Button pin for WipeMode

#define Password_Lenght 5 // Give enough room for six chars + NULL char
String number ="9968681026";
int randNumber;
int sgn = 12;


char Data[Password_Lenght]; // 6 is the number of chars it can hold + the null char = 7
//char Master[Password_Lenght] = "123456"; 
byte data_count = 0;
bool Pass_is_good;
char customKey;

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};


byte rowPins[ROWS] = {
  5,4,3,2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {
  8,7,6}; //connect to the column pinouts of the keypad


boolean match = false; // initialize card match to false
boolean programMode = false; // initialize programming mode to false

int successRead; // Variable integer to keep if we have Successful Read from Reader

int buttonPin = A0;
  

int buttonState = 0; 
byte storedCard[4];   // Stores an ID read from EEPROM
byte readCard[4];           // Stores scanned ID read from RFID Module
byte masterCard[4]; // Stores master card's ID read from EEPROM

/* We need to define MFRC522's pins and create instance
 * Pin layout should be as follows (on Arduino Uno):
 * MOSI: Pin 11 / ICSP-4
 * MISO: Pin 12 / ICSP-1
 * SCK : Pin 13 / ICSP-3
 * SS : Pin 10 (Configurable)
 * RST : Pin 9 (Configurable)
 * look MFRC522 Library for
 * pin configuration for other Arduinos.
 */

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.
Keypad customKeypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); //initialize an instance of class NewKeypad

///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {
  randomSeed(analogRead(A0));
  pinMode(sgn, INPUT_PULLUP);
  //Arduino Pin Configuration
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(relay, OUTPUT);

  pinMode(buttonPin, INPUT_PULLUP);
  
    Serial.begin(9600);
    Serial1.begin(9600);
  digitalWrite(relay, HIGH); // Make sure door is locked
  digitalWrite(redLed, LED_OFF); // Make sure led is off
  digitalWrite(greenLed, LED_OFF); // Make sure led is off
  digitalWrite(blueLed, LED_OFF); // Make sure led is off

  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  
  //Protocol Configuration
  Serial.begin(9600);  // Initialize serial communications with PC
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); //Set Antenna Gain to Max- this will increase reading distance

  //Wipe Code if Button Pressed while setup run (powered on) it wipes EEPROM
  pinMode(wipeB, INPUT_PULLUP);  // Enable pin's pull up resistor
  if (digitalRead(wipeB) == LOW) {     // when button pressed pin should get low, button connected to ground
    digitalWrite(redLed, LED_ON);   // Red Led stays on to inform user we are going to wipe
    Serial.println("!!! Wipe Button Pressed !!!");
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Wipe Button");
    lcd.setCursor(5, 1);
    lcd.print("Pressed");
    delay(1000);
    Serial.println("You have 5 seconds to Cancel");
    lcd.clear();
    lcd.setCursor(3, 0);
    lcd.print("You have 5");
    lcd.setCursor(1, 1);
    lcd.print("secs to Cancel");
    delay(1000);
    Serial.println("This will remove all records and cannot be undone");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("This will remove");
    lcd.setCursor(1, 1);
    lcd.print("All Records");
    delay(1000);
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("PERMANENTLY!");
    delay(5000);    // Give user enough time to cancel operation
    if (digitalRead(wipeB) == LOW) {  // If button still be pressed, wipe EEPROM
      Serial.println("!!! Starting Wiping EEPROM !!!");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Starting Wiping");
      lcd.setCursor(5, 1);
      lcd.print("EEPROM");
      delay(1000); 
      for (int x=0; x<1024; x=x+1){ //Loop end of EEPROM address
        if (EEPROM.read(x) == 0){ //If EEPROM address 0 
          // do nothing, already clear, go to the next address in order to save time and reduce writes to EEPROM
        } 
        else{
          EEPROM.write(x, 0); // if not write 0, it takes 3.3mS
        }
      }
      Serial.println("!!! Wiped !!!");
      lcd.clear();
      lcd.setCursor(5, 1);
      lcd.print("Wiped!");
      digitalWrite(redLed, LED_OFF); // visualize successful wipe
      delay(200);
      digitalWrite(redLed, LED_ON);
      delay(200);
      digitalWrite(redLed, LED_OFF);
      delay(200);
      digitalWrite(redLed, LED_ON);
      delay(200);
      digitalWrite(redLed, LED_OFF);
    }
    else {
      Serial.println("!!! Wiping Cancelled !!!");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wiping Cancelled");
      digitalWrite(redLed, LED_OFF);
      delay(750);
    }
  }
  //Check if master card defined, if not let user choose a master card
  //This also useful to just redefine Master Card
  //You can keep other EEPROM records just write other than 1 to EEPROM address 1
  if (EEPROM.read(1) != 1) {  // Look EEPROM if Master Card defined, EEPROM address 1 holds if defined
    Serial.println("No Master Card Defined");
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("No Master Card");
    lcd.setCursor(4, 1);
    lcd.print("Defined!");
    delay(1500);
    Serial.println("Scan A PICC to Define as Master Card");
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Scan A PICC to");
    lcd.setCursor(0, 1);
    lcd.print("Def. Master Card");
    do {
      successRead = getID(); // sets successRead to 1 when we get read from reader otherwise 0
      digitalWrite(blueLed, LED_ON); // Visualize Master Card need to be defined
      delay(200);
      digitalWrite(blueLed, LED_OFF);
      delay(200);
    }
    while (!successRead); //the program will not go further while you not get a successful read
    for ( int j = 0; j < 4; j++ ) { // Loop 4 times
      EEPROM.write( 2 +j, readCard[j] ); // Write scanned PICC's UID to EEPROM, start from address 3
    }
    EEPROM.write(1,1); //Write to EEPROM we defined Master Card.
    Serial.println("Master Card Defined");
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Master Card!");
    lcd.setCursor(4, 1);
    lcd.print("Defined!");
    delay(1000);
    
  }
  Serial.println("RFID Door Acces Control"); //For debug purposes
  Serial.println("Master Card's UID");
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Master Card's");
  lcd.setCursor(6, 1);
  lcd.print("UID!");
  delay(1500);
  for ( int i = 0; i < 4; i++ ) {     // Read Master Card's UID from EEPROM
    masterCard[i] = EEPROM.read(2+i); // Write it to masterCard
    Serial.print(masterCard[i], HEX);
  }
  Serial.println("");
  Serial.println("Waiting PICCs to be scanned :)");
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Waiting PICCs");
  lcd.setCursor(0, 1);
  lcd.print("to be scanned..");
  cycleLeds();    // Everything ready lets give user some feedback by cycling leds
}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {
    
    successRead = getID(); // sets successRead to 1 when we get read from reader otherwise 0
    if (programMode) {
      cycleLeds(); // Program Mode cycles through RGB waiting to read a new card
    }
    else {
      normalModeOn(); // Normal mode, blue Power LED is on, all others are off
    }

 if (successRead){
   if (programMode) {
    if ( isMaster(readCard) ) {  //If master card scanned again exit program mode
      Serial.println("This is Master Card"); 
      Serial.println("Exiting Program Mode");
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Master Card");
        lcd.setCursor(0, 1);
        lcd.print("Exiting Program!");
        delay(1500);
      Serial.println("-----------------------------");
      programMode = false;
      return;
    }
    else {  
      if ( findID(readCard) ) { //If scanned card is known delete it
        Serial.println("I know this PICC, so removing");
        deleteID(readCard);
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Known PICC");
        lcd.setCursor(2, 1);
        lcd.print("DELETING..");
        delay(1500);
        Serial.println("-----------------------------");
      }
      else {                    // If scanned card is not known add it
        Serial.println("I do not know this PICC, adding...");
        writeID(readCard);
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Unknown PICC");
          lcd.setCursor(4, 1);
          lcd.print("ADDING..");
          delay(1500);
        Serial.println("-----------------------------");
      }
    }
  }
  else {
    if ( isMaster(readCard) ) {  // If scanned card's ID matches Master Card's ID enter program mode
      programMode = true;
      Serial.println("Hello Master - Entered Program Mode");
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Hello Master");
        lcd.setCursor(0, 1);
        lcd.print("Entered Program!");
        delay(1500);
      int count = EEPROM.read(0); // Read the first Byte of EEPROM that
      Serial.print("I have ");    // stores the number of ID's in EEPROM
      Serial.print(count);
      Serial.print(" record(s) on EEPROM");
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("I have ");
        lcd.print(count);
        lcd.print("records");
        lcd.setCursor(3, 1);
        lcd.print("on EEPROM.");
        delay(1500);
      Serial.println("");
      Serial.println("Scan a PICC to ADD or REMOVE");
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Scan a PICC to");
        lcd.setCursor(1, 1);
        lcd.print("ADD or DELETE.");
        delay(1500);
      Serial.println("-----------------------------");
    }
    else {
      if ( findID(readCard) ) {        // If not, see if the card is in the EEPROM 
        Serial.println("Welcome, You shall pass");
        openDoor(1000);              // Open the door lock for 1000 ms
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Welcome");
          lcd.setCursor(1, 1);
          lcd.print("Access Granted");
          delay(1500);
      }
      else {        // If not, show that the ID was not valid
        Serial.println("You shall not pass");
        failed(); 
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("Unknown PICC");
          lcd.setCursor(1, 1);
          lcd.print("Access Denied!");
          delay(1500);
      }
    }
  }
 }
 else{
 if(digitalRead(buttonPin)== LOW)
 {
    randNumber = random(1000,9999);
    Serial.println(randNumber);

    
     Serial1.println("AT+CMGF=1"); // Set the Mode as Text Mode
      delay(150);
      Serial1.println("AT+CMGS=\"+91"+number+"\""); // Specify the Destination number in international format by replacing the 0's
      delay(150);
      Serial1.println("Your OTP is:"); // Enter the message 
      Serial1.println(randNumber);
      delay(150);
      Serial1.write((byte)0x1A); // End of message character 0x1A : Equivalent to Ctrl+z
      delay(50);
      Serial1.println();
          delay(1500);
 }
 customKey = customKeypad.getKey();
  if (customKey) // makes sure a key is actually pressed, equal to (customKey != NO_KEY)
  {
    Data[data_count] = customKey; // store char into data array
   // lcd.setCursor(data_count,1); // move cursor to show each new char
    //lcd.print(Data[data_count]); // print char at said cursor
    Serial.println(Data[data_count]);
    data_count++; // increment data array by 1 to store new char, also keep track of the number of chars entered
  }

  if(data_count == Password_Lenght-1) // if the array index is equal to the number of expected chars, compare data to master
  {
    //lcd.clear(); 
    //lcd.setCursor(0, 0);
    //lcd.print("Password is ");
    int i; 
    int k=0;
    for(i=0; i<4; i= i+1)
    {
      k = (10*k) + (Data[i]-48);
    }
Serial.println(k);
    if(k == randNumber)
    { // equal to (strcmp(Data, Master) == 0)
      //lcd.print("Good");
      Serial.println("Good");
       digitalWrite(blueLed, LED_OFF); // Turn off blue LED
  digitalWrite(redLed, LED_OFF); // Turn off red LED  
  digitalWrite(greenLed, LED_ON);

      }
     else
      {
      Serial.println("Bad");
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  digitalWrite(redLed, LED_ON);
      }

    delay(1000);// added 1 second delay to make sure the password is completely shown on screen before it gets cleared.
    //lcd.clear();
    clearData();   
    
 }
}

}


void clearData()
{
  while(data_count !=0)
  {   // This can be used for any array size, 
    Data[data_count--] = 0; //clear array for new data
  }
  return;
}
///////////////////////////////////////// Get PICC's UID ///////////////////////////////////
int getID() {
  // Getting ready for Reading PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { //If a new PICC placed to RFID reader continue
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) { //Since a PICC placed get Serial and continue
    return 0;
  }
  // There are Mifare PICCs which have 4 byte or 7 byte UID care if you use 7 byte PICC
  // I think we should assume every PICC as they have 4 byte UID
  // Until we support 7 byte PICCs
  Serial.println("Scanned PICC's UID:");
  for (int i = 0; i < 4; i++) {  // 
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA(); // Stop reading
  return 1;
}

///////////////////////////////////////// Cycle Leds (Program Mode) ///////////////////////////////////
void cycleLeds() {
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  delay(200);
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(blueLed, LED_ON); // Make sure blue LED is on
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  delay(200);
}

//////////////////////////////////////// Normal Mode Led  ///////////////////////////////////
void normalModeOn () {
  digitalWrite(blueLed, LED_ON); // Blue LED ON and ready to read card
  digitalWrite(redLed, LED_OFF); // Make sure Red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure Green LED is off
  digitalWrite(relay, HIGH); // Make sure Door is Locked
}

//////////////////////////////////////// Read an ID from EEPROM //////////////////////////////
void readID( int number ) {
  int start = (number * 4 ) + 2; // Figure out starting position
  for ( int i = 0; i < 4; i++ ) { // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start+i); // Assign values read from EEPROM to array
  }
}

///////////////////////////////////////// Add ID to EEPROM   ///////////////////////////////////
void writeID( byte a[] ) {
  if ( !findID( a ) ) { // Before we write to the EEPROM, check to see if we have seen this card before!
    int num = EEPROM.read(0); // Get the numer of used spaces, position 0 stores the number of ID cards
    int start = ( num * 4 ) + 6; // Figure out where the next slot starts
    num++; // Increment the counter by one
    EEPROM.write( 0, num ); // Write the new count to the counter
    for ( int j = 0; j < 4; j++ ) { // Loop 4 times
      EEPROM.write( start+j, a[j] ); // Write the array values to EEPROM in the right position
    }
    successWrite();
  }
  else {
    failedWrite();
  }
}

///////////////////////////////////////// Remove ID from EEPROM   ///////////////////////////////////
void deleteID( byte a[] ) {
  if ( !findID( a ) ) { // Before we delete from the EEPROM, check to see if we have this card!
    failedWrite(); // If not
  }
  else {
    int num = EEPROM.read(0); // Get the numer of used spaces, position 0 stores the number of ID cards
    int slot; // Figure out the slot number of the card
    int start;// = ( num * 4 ) + 6; // Figure out where the next slot starts
    int looping; // The number of times the loop repeats
    int j;
    int count = EEPROM.read(0); // Read the first Byte of EEPROM that stores number of cards
    slot = findIDSLOT( a ); //Figure out the slot number of the card to delete
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--; // Decrement the counter by one
    EEPROM.write( 0, num ); // Write the new count to the counter
    for ( j = 0; j < looping; j++ ) { // Loop the card shift times
      EEPROM.write( start+j, EEPROM.read(start+4+j)); // Shift the array values to 4 places earlier in the EEPROM
    }
    for ( int k = 0; k < 4; k++ ) { //Shifting loop
      EEPROM.write( start+j+k, 0);
    }
    successDelete();
  }
}

///////////////////////////////////////// Check Bytes   ///////////////////////////////////
boolean checkTwo ( byte a[], byte b[] ) {
  if ( a[0] != NULL ) // Make sure there is something in the array first
    match = true; // Assume they match at first
  for ( int k = 0; k < 4; k++ ) { // Loop 4 times
    if ( a[k] != b[k] ) // IF a != b then set match = false, one fails, all fail
      match = false;
  }
  if ( match ) { // Check to see if if match is still true
    return true; // Return true
  }
  else  {
    return false; // Return false
  }
}

///////////////////////////////////////// Find Slot   ///////////////////////////////////
int findIDSLOT( byte find[] ) {
  int count = EEPROM.read(0); // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) { // Loop once for each EEPROM entry
    readID(i); // Read an ID from EEPROM, it is stored in storedCard[4]
    if( checkTwo( find, storedCard ) ) { // Check to see if the storedCard read from EEPROM
      // is the same as the find[] ID card passed
      return i; // The slot number of the card
      break; // Stop looking we found it
    }
  }
}

///////////////////////////////////////// Find ID From EEPROM   ///////////////////////////////////
boolean findID( byte find[] ) {
  int count = EEPROM.read(0); // Read the first Byte of EEPROM that
  for ( int i = 1; i <= count; i++ ) {  // Loop once for each EEPROM entry
    readID(i); // Read an ID from EEPROM, it is stored in storedCard[4]
    if( checkTwo( find, storedCard ) ) {  // Check to see if the storedCard read from EEPROM
      return true;
      break; // Stop looking we found it
    }
    else {  // If not, return false   
    }
  }
  return false;
}

///////////////////////////////////////// Write Success to EEPROM   ///////////////////////////////////
// Flashes the green LED 3 times to indicate a successful write to EEPROM
void successWrite() {
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is on
  delay(200);
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  delay(200);
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  delay(200);
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(greenLed, LED_ON); // Make sure green LED is on
  delay(200);
  Serial.println("Succesfully added ID record to EEPROM");
}

///////////////////////////////////////// Write Failed to EEPROM   ///////////////////////////////////
// Flashes the red LED 3 times to indicate a failed write to EEPROM
void failedWrite() {
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  delay(200);
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  delay(200);
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  delay(200);
  digitalWrite(redLed, LED_ON); // Make sure red LED is on
  delay(200);
  Serial.println("Failed! There is something wrong with ID or bad EEPROM");
}

///////////////////////////////////////// Success Remove UID From EEPROM  ///////////////////////////////////
// Flashes the blue LED 3 times to indicate a success delete to EEPROM
void successDelete() {
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  digitalWrite(redLed, LED_OFF); // Make sure red LED is off
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  delay(200);
  digitalWrite(blueLed, LED_ON); // Make sure blue LED is on
  delay(200);
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  delay(200);
  digitalWrite(blueLed, LED_ON); // Make sure blue LED is on
  delay(200);
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  delay(200);
  digitalWrite(blueLed, LED_ON); // Make sure blue LED is on
  delay(200);
  Serial.println("Succesfully removed ID record from EEPROM");
}

////////////////////// Check readCard IF is masterCard   ///////////////////////////////////
// Check to see if the ID passed is the master programing card
boolean isMaster( byte test[] ) {
  if ( checkTwo( test, masterCard ) )
    return true;
  else
    return false;
}

///////////////////////////////////////// Unlock Door   ///////////////////////////////////
void openDoor( int setDelay ) {
  digitalWrite(blueLed, LED_OFF); // Turn off blue LED
  digitalWrite(redLed, LED_OFF); // Turn off red LED  
  digitalWrite(greenLed, LED_ON); // Turn on green LED
  digitalWrite(relay, LOW); // Unlock door!
  delay(setDelay); // Hold door lock open for given seconds
  digitalWrite(relay, HIGH); // Relock door
  delay(2000); // Hold green LED on for 2 more seconds
}

///////////////////////////////////////// Failed Access  ///////////////////////////////////
void failed() {
  digitalWrite(greenLed, LED_OFF); // Make sure green LED is off
  digitalWrite(blueLed, LED_OFF); // Make sure blue LED is off
  digitalWrite(redLed, LED_ON); // Turn on red LED
  delay(1200);
}

