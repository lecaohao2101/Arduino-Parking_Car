/*New blynk app project
   Home Page
*/
#include <Servo.h>
Servo myservo1;
//Include the library files
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <BlynkSimpleEsp8266.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// #include <Wire.h> 
// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x3F,16,2);  //Change the HEX address  //Change the HEX address
#define BLYNK_TEMPLATE_ID "TMPLCXGLXZZF"
#define BLYNK_DEVICE_NAME "Parking Car"
#define BLYNK_AUTH_TOKEN "w_DqqDomw0QoOgtFcsI8ja-_fTzM1bds"
//Enter your blynk auth token
// Define the API key and URL for Firebase database
// Notice: replace your API key and database URL 
#define API_KEY "AIzaSyDHI0cMRAzym1JjzloRKIth5TiBuMw9l0Y"
#define DATABASE_URL "https://parking-car-af500-default-rtdb.asia-southeast1.firebasedatabase.app/"
char authblynk[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Vinhtailong_2G";//Enter your WIFI name
char pass[] = "ly612520";//Enter your WIFI password


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
WiFiManager wifiManager;
bool signupOK = false;
String getDatetime(){
 	timeClient.update();
 	time_t epochTime = timeClient.getEpochTime();
 	struct tm *ptm = gmtime ((time_t *)&epochTime);
 	int monthDay = ptm->tm_mday;
 	int currentMonth = ptm->tm_mon+1;
 	int currentYear = ptm->tm_year+1900;
 	String formattedTime = timeClient.getFormattedTime();
 	return String(monthDay) + "-" + String(currentMonth) + "-" +
String(currentYear) + " " + formattedTime;
}


int value;
#define IR1 D1
#define IR2 D2

int flag1 = 0;
int flag2 = 0;
int Slot = 4;           //Enter Total number of parking Slots

//Get the button value
BLYNK_WRITE(V0) {
  digitalWrite(D3, param.asInt());
  value= param.asInt();
}

void setup() {
  //Set the LED pin as an output pin
  Serial.begin(9600);
  timeClient.begin();
 timeClient.setTimeOffset(25200);
  pinMode(D3, OUTPUT);
  pinMode(D0, OUTPUT);
  pinMode(IR1, INPUT);
pinMode(IR2, INPUT);
  myservo1.attach(D0);
myservo1.write(100);
Serial.println("     ARDUINO    ");
Serial.println(" PARKING SYSTEM ");
delay (2000);
 bool res = wifiManager.autoConnect("NamAP");
 	if (!res) Serial.println("Failed to connect");
 	else Serial.println("Connected...yay ");
 	Serial.println("Connecting to Wi-Fi");
 	Serial.println("Connected with IP: ");
	// Assign the api key (required) 
 config.api_key = API_KEY;
// Assign the RTDB URL (required) 
 config.database_url = DATABASE_URL;
/* 
Sign up on Firebase
In this lab, the database is opened for anonymous 
For a better security, you need to setup username and password
And fill it in the statement Firebase.signUp
*/
 if (Firebase.signUp(&config, &auth, "", "")){
 Serial.println("ok");
 signupOK = true;
 }
 else{
 Serial.printf("%s\n", config.signer.signupError.message.c_str());
 }

//Assign the callback function for the long running token generation task
 config.token_status_callback = tokenStatusCallback;
 Firebase.begin(&config, &auth);
 Firebase.reconnectWiFi(true);

  //Initialize the Blynk library
  Blynk.begin(authblynk, ssid, pass, "blynk.cloud", 80);
}

//define path in the database
String prPath = "/ASM2";
String timePath = "/ASM2";
String databasePath = "/Asm";
String parentPath;


void loop() {

 Blynk.run();
  int pr_value =Slot;

    if(digitalRead (IR1) == LOW && flag1==0){
    if(Slot>0){flag1=1;
    if(flag2==0){myservo1.write(0); Slot = Slot-1;}
    }else{
    Serial.println("    SORRY :(    ");  
    Serial.println("  Parking Full  "); 
    delay (3000);
    }
    }

    if(digitalRead (IR2) == LOW && flag2==0){flag2=1;
    if(flag1==0){myservo1.write(0); Slot = Slot+1;}
    }

    if(flag1==1 && flag2==1){
    delay (100);
    myservo1.write(100);
    flag1=0, flag2=0;
    }

    Serial.println("    WELCOME!    ");
    Serial.println("Slot Left: ");
    Serial.println(Slot);
    // firebase
    if (Firebase.ready() && signupOK){
 		delay(100);
String datetime = getDatetime();
 		Serial.print ("time: ");
 		parentPath= databasePath + "/" + datetime;
 		//set the JSON string
 		json.set(timePath.c_str(), pr_value);
 		//send data to the real-time database
 		if (Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json)){
 			Serial.println("PASSED");
 			Serial.println("PATH: " + fbdo.dataPath());
Serial.println("TYPE: " + fbdo.dataType());
 		}
 		else {
 			Serial.println("FAILED");
 			Serial.println("REASON: " + fbdo.errorReason());
 		}
}

}
