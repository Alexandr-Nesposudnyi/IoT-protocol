// device A
#include <SoftwareSerial.h>
SoftwareSerial softSerial(8,9);
SoftwareSerial softSerialC (12,13);
SoftwareSerial softSerialD(16,17);
bool newDataServer = true;
bool newDataC = true;
bool newDataD = true;
int subaddress[2][2] = {{0,0},{0,0}};

byte nodeaddress = 101;
byte sensornode = 0;
int requestgiveSensor = 76;
typedef struct messageData_t {
  int request;
  int codesensor;
  int body;
  int checksum;
};

byte  codesOfSensors[] = {1,2,3,4};
/*-----
1 - LED
2 - temp
3 - motor
------*/


typedef union Message {
 messageData_t message;
 byte MessageBytes[sizeof(message)];
};

Message IN;
Message OUT;

void setup()
{
  softSerial.begin(4800);
  softSerialC.begin(4800);
   softSerialD.begin(4800);
  Serial.begin(9600);
 
    pinMode(7, OUTPUT);

}
void loop(){
   delay(1000);
 
  if(newDataServer == true){
 sendIformationofSensor(codesOfSensors[0],nodeaddress);
    delay(2000);
   
}
   softSerial.listen();
  delay(2000);
  int readStatus = getDataFromStream(softSerial);
  if(readStatus == 0){
    if(IN.message.request == 100){
       Serial.print("Request ");
        Serial.println(IN.message.request);
         Serial.print("Sensor ");
        Serial.println(IN.message.codesensor);
         Serial.print("Address ");
        Serial.println(IN.message.body);
       Serial.print("checksum "); 
        Serial.println(IN.message.checksum); 
  Serial.println("OK"); 
      Serial.println("----------------------");
      if(IN.message.body == 103){
      newDataC = false;
        subaddress[0][0] = IN.message.body;
        subaddress[0][1] = IN.message.codesensor;
        Serial.println("Address 103 saved");
       SendConfirmation(IN.message.codesensor,IN.message.body,softSerialC);

          delay(1000);

      }
    }
    if(IN.message.request == 78){
     digitalWrite(7, HIGH);
      Serial.print("Request ");
        Serial.println(IN.message.request);
         Serial.print("Sensor ");
        Serial.println(IN.message.codesensor);
         Serial.print("Address ");
        Serial.println(IN.message.body);
       Serial.print("checksum "); 
        Serial.println(IN.message.checksum); 
      Serial.println("----------------------");
   delay(2000);
    }
    newDataServer = false;
  }
  
  delay(500);
  
  if(newDataC == true && newDataServer == false){
  softSerialC.listen();
  delay(500);
  
   readStatus = getDataFromStream(softSerialC);
 if(readStatus == 0 && newDataC == true){
   Serial.println("Data from C device:");
   if(IN.message.request == requestgiveSensor){
    Serial.print("Device C has a sensor :");
    Serial.println(IN.message.codesensor);
    Serial.print("Request ");
        Serial.println(IN.message.request);
         Serial.print("Sensor ");
        Serial.println(IN.message.codesensor);
         Serial.print("Address ");
        Serial.println(IN.message.body);
       Serial.print("checksum "); 
        Serial.println(IN.message.checksum); 
     Serial.println("----------------------");
     sendIformationofSensor(IN.message.codesensor,IN.message.body);
    
     // newDataC = false;
   }
    }
  }
  // Data from D device
  delay(600);
  if(newDataD == true && newDataServer == false ){
 softSerialD.listen();
   
    delay(600);
    readStatus = getDataFromStream(softSerialD);
    if(readStatus == 0){
      if(IN.message.request == 77 && newDataC == false){
        Serial.print("Device ");
        Serial.print(IN.message.body);
        Serial.print(" ask for sensor ");
        Serial.println(IN.message.codesensor);
        if(subaddress[0][1] == IN.message.codesensor){
         Serial.print("Device ");
         Serial.print(subaddress[0][0]);
         Serial.print(" has such a sensor ");
          SendConfirmation(IN.message.codesensor,IN.message.body,softSerialD);
        }
        
      }
      if(IN.message.request == 78){
      //  Serial.println("Data from stream");
        
        OUT.message.request = IN.message.request;
        OUT.message.codesensor = IN.message.codesensor;
        OUT.message.body = IN.message.body;
        OUT.message.checksum = OUT.message.request + OUT.message.codesensor + OUT.message.body;
       // if(IN.message.body == 103){
        Serial.print("Request : ");
        Serial.println(OUT.message.request);
        Serial.println("Send data");
          writeDataToStream(softSerialC);
        delay(300);

        //}

      }
    }
  }
}
  void sendIformationofSensor(int sensor,int address){
   OUT.message.request = requestgiveSensor;
   OUT.message.codesensor = sensor;
  OUT.message.body = address;
  OUT.message.checksum = OUT.message.request + OUT.message.codesensor + OUT.message.body ;
    writeDataToStream(softSerial);
    
  
}
int writeDataToStream(Stream &softSerial) {
  int count = 0;
  for (int i=0; i<sizeof(OUT); i++) {
    softSerial.write(OUT.MessageBytes[i]);
    count++;
  }
}
  int getDataFromStream(Stream &softSerial){
  byte byteArray[sizeof(IN)];
  if (softSerial.available() >= sizeof(IN) ){
    for (int i=0; i<sizeof(IN); i++) {
      byteArray[i] = softSerial.read();
    }
  }
  for (int i=0; i<sizeof(IN); i++) {
  IN.MessageBytes[i] = byteArray[i];
  }
    
  if(IN.message.request + IN.message.codesensor + IN.message.body == IN.message.checksum) {
    return 0;
  }
  else return -1;
  }
  
  void  SendConfirmation(int data,int address,Stream &softSerial){
   OUT.message.request = 100;
   OUT.message.codesensor = data;
  OUT.message.body = address;
  OUT.message.checksum = OUT.message.request +OUT.message.codesensor + OUT.message.body;
  writeDataToStream(softSerial);
    Serial.println("Send confirmation");
    Serial.println("----------------------");

}
