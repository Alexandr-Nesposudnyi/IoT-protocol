// Server

#include <SoftwareSerial.h>
SoftwareSerial SerialA(8, 9);
SoftwareSerial SerialB(12, 13);
SoftwareSerial softSerialLock(16,17);

int addressPool[] = {101,102,103,104,105,106,107,108};
int saveSensors[8][5];
int countOfAddress = 0;
int sensor = 0;
int request = 54;
bool newdata = true;
bool newdataC = true;
int addressA = 101;
bool lock = false;
int addressB = 102;
byte  codesOfSensors[] = {1,2,3,4};

/*-----
1 - LED
2 - temp
3 - motor
------*/
typedef struct messageData_t {
  int request;
  int codesensor;
  int body;
  int checksum;
};

typedef union Message {
 messageData_t message;
 byte MessageBytes[sizeof(message)];
};

Message IN;
Message OUT;
void setup()
{
  Serial.begin(9600);
  SerialA.begin(4800);
  SerialB.begin(4800);
  softSerialLock.begin(4800);
  cleanTheTableofSensors();
}

void loop(){
  if(lock == false ){
    softSerialLock.listen();
   
    int readStatus = getDataFromStream(softSerialLock);
    delay(1000);
    if(readStatus == 0){
     Serial.println("Data From Lock");
     Serial.println("Home is working");
     Serial.println("----------------------");
      lock = true;
    }
  }
  else{
  SerialA.listen();
  delay(2000);
  int readStatus = getDataFromStream(SerialA);
  if (readStatus == 0) {
    if(IN.message.request == 76 && newdata == true){
      Serial.print("Transmission of information about a sensor, from device ");
      delay(2000);
 
      Serial.println(IN.message.body);
      
     // newdata = false;
      int readStatusSensor = saveSensorData(IN.message.body,IN.message.codesensor);
      
      if(readStatusSensor == 0){
       Serial.print("Request ");
        Serial.println(IN.message.request);
         Serial.print("Sensor ");
        Serial.println(IN.message.codesensor);
         Serial.print("Address ");
        Serial.println(IN.message.body);
       Serial.print("checksum "); 
        Serial.println(IN.message.checksum); 
       Serial.println("Sensor saved ");
     Serial.println("----------------------");
        SendConfirmation(IN.message.codesensor,IN.message.body); 
      }
      else{
      Serial.println("Sensor not saved");
      Serial.println("----------------------");

      }
}


  
}
  
  delay(200);
   SerialB.listen();
 delay(1000);
    readStatus = getDataFromStream(SerialB);
  if(readStatus == 0){
   
    if(IN.message.request == 77){
    
      int readSensorAnswer = giveSensor(102,IN.message.codesensor);
      if(readSensorAnswer!= 0){
      Serial.print("Device B ask for Sensor : ");
      Serial.println(IN.message.codesensor);
      Serial.print("Device ");
      Serial.print(readSensorAnswer);
      Serial.println(" has such the sensor");
      Serial.println("----------------------");

       
      SendConfirmationB(readSensorAnswer);
    }
      
    }
       if(IN.message.request == 78){
     Serial.println("Device B send Data to sensor");
        Serial.print("Request ");
        Serial.println(IN.message.request);
         Serial.print("Sensor ");
        Serial.println(IN.message.codesensor);
         Serial.print("Address ");
        Serial.println(IN.message.body);
       Serial.print("checksum "); 
        Serial.println(IN.message.checksum); 
        Serial.println("----------------------");

        OUT.message.request = 78;
        OUT.message.codesensor = codesOfSensors[0];
        OUT.message.body = 102;
        OUT.message.checksum = OUT.message.request + OUT.message.codesensor + OUT.message.body;
         writeDataToStream(SerialA);
  
       }
    delay(100);
  }
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

  int saveSensorData (int address, int sensor){
  int i = 0;
  while(i < 8){
    if(saveSensors[i][0] == address){
    for(int j = 1; j < 5;j++){
      if(saveSensors[i][j] == 0){
       saveSensors[i][j] = sensor;
        j = 6;
return 0;        
      }
      else{
return -1;         
  }
     
    }

}
                i++;

  }
  }
   void  SendConfirmation(int data,int address){
   OUT.message.request = 100;
   OUT.message.codesensor = data;
  OUT.message.body = address;
  OUT.message.checksum = OUT.message.request +OUT.message.codesensor + OUT.message.body;
  writeDataToStream(SerialA);
}
  
  
  
  
  
  
void  SendConfirmationB(int data){
   OUT.message.request = 100;
  OUT.message.codesensor = data;
  OUT.message.body = 102;
  OUT.message.checksum = OUT.message.request + OUT.message.codesensor +  OUT.message.body;
  writeDataToStream(SerialB);
}
  
  
  
void cleanTheTableofSensors(){
 for(int i = 0 ; i < 8;i++){
    for(int j = 0; j < 5; j++){
     saveSensors[i][j] = 0;
    }
   
   for(int i = 0 ; i<8;i++){
    saveSensors[i][0] = addressPool[i];
   }
   
 }
}

  
  
  
  int writeDataToStream(Stream &softSerial) {
  int count = 0;
  for (int i=0; i<sizeof(OUT); i++) {
    softSerial.write(OUT.MessageBytes[i]);
    count++;
  }
}
  int giveSensor(int address,int sensor){
 int i = 0;
  while(i < 8){
    if(saveSensors[i][0] != address){
      for(int j = 1;j < 5; j++){
        if(saveSensors[i][j] == sensor){
         return saveSensors[i][0]; 
          
          j = 6;
        }
       
      }
    }
    i++;
  }
}
  
    
    
 
