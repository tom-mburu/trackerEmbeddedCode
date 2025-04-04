#include <Arduino.h>
//required libraries
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <TinyGPSPlus.h>
#include <LiquidCrystal.h>


// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected toa
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
/*class sim800lgsm{
  private:
  String receivedRequest;
  String setToTextMode="AT+CMGF=1";

  String sendingTextCommand="AT+CMGS=\"+2540700398081\""; 
  //Decides how newly arrived SMS messages should be handled
  String treatModeFoReceivedSms="AT+CNMI=1,2,0,0,0";
  public:
  sim800lgsm(String handshake){
    lcd.print("in constructer");
    
    //inititialize the sim800l module 
    repeat:
    Serial.print(handshake);
    while(!(Serial.available()>0)){
lcd.print("waiting 'AT'");
delay(500);
lcd.clear();
    };//wait for response
    String response=Serial.readString();
    
    if(response.equalsIgnoreCase("ok")){
      //then its all good to go .
      lcd.clear();
      lcd.print("AT okay");
    }else{
      lcd.clear();
      lcd.print("AT failed!");
       goto repeat;//go to initialization again.
    }
    }
  
  //method for sending sms
  int  sendSms(String msg){
//set text mode
setMode:
Serial.println(setToTextMode);
while(!(Serial.available()>0));//wait for response
if(Serial.readString().indexOf("OK")>=0){
  //continue since text mode is set
  lcd.clear();
  lcd.print("text mode ok.");
}else{
  lcd.clear();
  lcd.print("fail settxtmode!");
goto setMode;
}
//set the phone number to send sms to
textCommand:
Serial.println(sendingTextCommand);
while(!(Serial.available()>0));//block while waiting for response
if(Serial.readString().indexOf("OK")>=0){
  //phone number sent to the sim800l module
  lcd.clear();
  lcd.print("phone numbersent");
  }else{
    lcd.clear();
    lcd.print("failed sendnumber");
    goto textCommand;
  }
  //sending the sms
  Serial.println(msg);
  Serial.write(26);//tell the module to send the message
  return 1;
}

//method for receiving the message 
void setModeForReceive(){
  //set to text mode
setMode:
Serial.println(setToTextMode);
while(!(Serial.available()>0));//wait for response
if(Serial.readString().indexOf("OK")>=0){
  //continue since text mode is set
  lcd.clear();
  lcd.print("set totextmode");
}else{
  lcd.clear();
  lcd.print("fail tosettxtmode");
goto setMode;
}
//setting how to handle received messages 
handleReceiveMode:
Serial.println(treatModeFoReceivedSms);
while(!(Serial.available()>0));//wait for response
if(Serial.readString().equalsIgnoreCase("ok")){
  //then good to go ...
  lcd.print("ready torecvmsg");
}else{
  lcd.println("fail sethowtorecv");
  goto handleReceiveMode;
}
}
String recvMsg(){
if(Serial.available()>0){//read data in the serial ports
this->receivedRequest=Serial.readString();
Serial.println("sms received ");
}else{
  Serial.println("no smsreceived");
}
return receivedRequest;
}
};*/

// put function declarations here:




//variable declaration 
#define rx 9
#define tx 10
#define engineSwitch 7
#define statusLedPin 8
String request;



//object instants
TinyGPSPlus gps;
SoftwareSerial sim800l(rx,tx);
//sim800lgsm GsmComObject("AT");

struct gpsdata{
  //structure for holding gps information
  String currentLatitude;
  String currentLongitude;
  String currentDate;
  String currentTime;
  String currentAltitude;
  String currentGroundSpeed;
  String numberOfSatellites;
  String googleMapLink;
  };

gpsdata currentLocation;//structure variable for holding gps info
//functions for gsm/gprs module

void testGsmGprs(){
  //test the sim800l
  testing:
  sim800l.println("AT");
  delay(500);
  while(!sim800l.available()){
    lcd.clear();
    lcd.print("waiting ATresponse");
    Serial.println("waiting AT response ");
  }//wait response
  if(sim800l.readString().indexOf("OK")>=0){
    lcd.clear();
    lcd.print("AT OKY");
    Serial.println("AT oky ");
  }else{
    lcd.clear();
    lcd.println("AT failed");
    Serial.println("AT failed .");
    delay(500);
    goto testing;
  }
  //setting text mode
  settextmode:
  sim800l.println("AT+CMGF=1");
  delay(500);
   while(!sim800l.available()){
    lcd.clear();
    lcd.print("waiting ATresponse");
    Serial.println("waiting for AT response ");
    delay(500);
  }//wait response
  if(sim800l.readString().indexOf("OK")>=0){
    lcd.clear();
    lcd.print("sms mode ok");
    Serial.println("sms mode is oky ");
    delay(500);
  }else{
    lcd.clear();
    lcd.print("sms modefail");
    Serial.println("sms mode failed ");
    delay(500);
    goto settextmode;
  }
 
}
void settingSmsRecvFormat(){
  setsmsrecvformat:
  sim800l.println("AT+CNMI=1,2,0,0,0");
  delay(500);
  while(!sim800l.available()){
    lcd.clear();
    lcd.print("waiting rcvmode");
    Serial.println("waiting for receive mode ");
    delay(500);
  }
  if(sim800l.readString().indexOf("OK")){
    lcd.clear();
    lcd.print("sms recvmodeset");
    Serial.println("sms receive mode set");
    delay(500);
  }else{
    lcd.clear();
    lcd.print("fail recvmodeset");
    delay(500);
    Serial.println("recv mode failed to set");
    goto setsmsrecvformat;
  }
}
void sendSms(String msg){
  //sending phone number to send sms to .
  setphoneNo:
  sim800l.println("AT+CMGS=\"+2540700398081\"");
  delay(500);
  while(!sim800l.available()){
    lcd.clear();
    lcd.print("sending no.");
    Serial.println("sendingphone number");
    delay(500);

  }
  if(sim800l.readString().indexOf("OK")>=0){
    lcd.clear();
    lcd.print("phone no sent");
    Serial.println("phone number sent to the module");
    delay(500);
  }else{
    lcd.clear(); 
    lcd.print("fail to sent no.");
    Serial.println("failed to send number");
    delay(500);
    goto setphoneNo;
  }
   sim800l.print(msg);
  sim800l.write(26);//send the msg.
  settingSmsRecvFormat();//resetting to recv mode after sending sms 
} 

String recvSms(){
  String receivedSms;
  if(sim800l.available()>0){
    receivedSms=sim800l.readString();
    }
    Serial.println(receivedSms);
  return receivedSms;
}

//function for oswitching on and off the engine 
int SwitchOnOffEngine(String command){
if(command.equalsIgnoreCase("on")){
  digitalWrite(engineSwitch,HIGH);
  return 1;
}else if(command.equalsIgnoreCase("off")){
  digitalWrite(engineSwitch,LOW);
  return 0;
} else{
  digitalWrite(engineSwitch,LOW);
}
return 0;
}

//function for obtaining gps data 
void getGpsData(){

  //funelling the gps object
  while(Serial.available()>0){
    char gpsChar=Serial.read();
    gps.encode(gpsChar);
  }
  //getting location 
  if(gps.location.isValid()){
    //get latitude and longitude
    currentLocation.currentLatitude=String(gps.location.lat(),6);
    currentLocation.currentLongitude=String(gps.location.lng(),6);
    }else{
Serial.println("invalid location data ");
currentLocation.currentLatitude="invalid latitude";
currentLocation.currentLongitude="invalid longitude";
    }
    if(gps.date.isValid()){
      String DAY=String(gps.date.day());
      String MONTH=String(gps.date.month());
      String YEAR=String(gps.date.year());
      currentLocation.currentDate=DAY+String('/')+MONTH+String('/')+YEAR;

    }else{
      Serial.println("invalid date");
      currentLocation.currentDate="invalid date ";
    }
    if(gps.time.isValid()){
      String HOUR=String(gps.time.hour());
      String MINUTE=String(gps.time.minute());
      String SECOND=String(gps.time.second());
      currentLocation.currentTime=HOUR+String(':')+MINUTE+String(':')+SECOND;

    }else{
      Serial.println("invalid time");
      currentLocation.currentTime="invalid time";
    }
    if(gps.altitude.isValid()){
      currentLocation.currentAltitude=String(gps.altitude.meters());
    }else{
      Serial.println("invalid altitude ");
      currentLocation.currentAltitude="invalid altitude";
    }
    if(gps.speed.isValid()){
      currentLocation.currentGroundSpeed=String(gps.speed.kmph());
    }else{
      Serial.println("invalid speed");
      currentLocation.currentGroundSpeed="invalid speed";
    }
    if(gps.satellites.isValid()){
    currentLocation.numberOfSatellites=String(gps.satellites.value());
    }else{
      currentLocation.numberOfSatellites="invalid number of satellites ";
    }
  
  }
  void blinkStatusLed(){
    for(int x=0;x<=2;x++){
      for(int y=0;y<=2;y++){
        digitalWrite(statusLedPin,HIGH);
        delay(100);
        digitalWrite(statusLedPin,LOW);
        delay(100);
      }
      delay(500);
    }
  }


void setup(){
  
 sim800l.begin(9600); 
 Serial.begin(9600);
 pinMode(statusLedPin,OUTPUT);
 //SETTING SIM800L INTO recv mode initially :
 lcd.begin(16,2);
 Serial.println("in setup() ");
 //GsmComObject.setModeForReceive();
 //testGsmGprs();
 //settingSmsRecvFormat();
  }



void loop(){
 // recvSms();
 /*if(Serial.available()){
  String messagerecv=Serial.readString();
 if( messagerecv.equalsIgnoreCase("location")){
//getGpsData();
sim800l.println("requesting location information .");
sim800l.write(26);
Serial.println("you requested location");
 }
 }
lcd.print("in loop()");
  blinkStatusLed();//for indicating ongoing activities
  
  //check for any data received in the hardware serial port
  //request=GsmComObject.recvMsg();
  if(request.indexOf("on")>=0){
int response=SwitchOnOffEngine("on");
if(response==1){
  //GsmComObject.sendSms("OK");
}
  }else if(request.indexOf("off")>=0){
    int response=SwitchOnOffEngine("off");
    if(response==1){
      //GsmComObject.sendSms("OK");
    }
  }
  if(request.indexOf("location")){
    String locResponse=currentLocation.currentLatitude+String("lat/")+
    currentLocation.currentLongitude+String("lng/")+currentLocation.currentDate+String("date/")+
    currentLocation.currentTime+String("time/")+currentLocation.currentGroundSpeed+
    String("gndSpeed/")+currentLocation.currentAltitude+String("alt/")+
    currentLocation.numberOfSatellites+String("sat/");
   // GsmComObject.sendSms(locResponse);
  }*/
  //printing location
  getGpsData();
  Serial.print("latitude:");
Serial.print(currentLocation.currentLatitude);
Serial.println();
Serial.print("longitude: ");
Serial.print(currentLocation.currentLongitude);
Serial.println();
//printing date
Serial.println(String("date : ")+currentLocation.currentDate);
Serial.println(String("current time:")+currentLocation.currentTime);
Serial.println(String("ground speed :")+currentLocation.currentGroundSpeed);
Serial.println(String("current altitude :")+currentLocation.currentAltitude);
Serial.println(String("sattelliets connected: ")+currentLocation.numberOfSatellites);
currentLocation.googleMapLink="http://www.google.com/maps/place/"+ currentLocation.currentLatitude + "," + currentLocation.currentAltitude;


delay(500);
}


//commands to send to the microcontoller
//on->for switching on the engine
//off-> for switching off the engine 
//location->lat,lng,date,time,sat,altitude

