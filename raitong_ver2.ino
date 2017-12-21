#include <SoftwareSerial.h>
#include "Adafruit_FONA.h"

#define FONA_RST 5
Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

//USER SETTINGS
const unsigned int LOGINTERVAL = 29000;//max of 65000, the interval in ms between logging of GPS data. Xiaomi logs off after 30sec
const unsigned long IDLEDURATION = 180000;//3 min idle time for stationary lorry before triggering an alert sms
String BenMobileNum = "0620216838", BryanMobileNum = "0859150961";
int SMSquota = 200; //number of SMSes left, 3 baht per sms
unsigned long lastSentAlertSMSTime;

//button switch
const int buttonPin = 7; //the other button pin is connected to ground
int buttonReading, buttonState;
int lastButtonState = HIGH; //HIGH by default
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 5000;
int ledState = LOW;
const int ledPin = 13; //the LED on board the Arduino

//fona sms
SoftwareSerial fonaSS(10, 11); // RX, TX
SoftwareSerial *fonaSerial = &fonaSS;
char replybuffer[255];
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);
char fonaNotificationBuffer[64];          //for notifications from the FONA
char smsBuffer[250]; //to store the received sms message content
char replyMsg[128]; //to store the reply message to send
String googleMapsQueryString = "http://maps.google.com/?q=";
unsigned long lastMovingTime;
bool isIdle;

//GPS
String latlon = "";
String speedKPH = "";

//SD card
#include <SPI.h>
#include <SD.h>
const int chipSelect = 4;
unsigned long lastLoggedTime;

//Tripwire
const int tripButtonPin = 2;//tripwire pin
int tripState = LOW;//variable for reading tripwire status
bool hasTripped=false;
long hasTrippedTime;

void setup()
{
  // Open serial communications to computer
  Serial.begin(115200); //NOTE: default: 115200, change to 9600 for serial monitor keyboard communication.

  //  setup_button(); //tact button for testing purposes. Not used.
  
  setup_tripButton();

  initialise_fona();

  setup_SDcard();
}

void loop()
{
  //  button_operation(); //test whether tact button pressed. Not used.

  //    run_serial_ports(); //for testing purposes only

  run_tripwire();

  run_sms_system();

  log_data();
}

void run_serial_ports() {
  //This loop just takes whatever comes in from the console and sends it to the board

  if (Serial.available())
  {
    fonaSS.write(Serial.read());
  }
  if (fonaSS.available())
  {
    Serial.write(fonaSS.read());
  }
}







