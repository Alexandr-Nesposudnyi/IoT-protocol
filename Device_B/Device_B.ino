//device B
#include <SoftwareSerial.h>
SoftwareSerial softSerial(12,13);

bool newdata = true;
byte nodeaddress = 102;
int requestasksensor = 77;
int senddatatosensor = 78;
bool foundSensor = false;
const int PIN_BUTTON = 2;

typedef struct messageData_t {
  int request;
  int codesensor;
  int body;
  int checksum;
};
byte  codesOfSensors[] = {1,2,3,4};

typedef union Message {
 messageData_t message;
 byte MessageBytes[sizeof(message)];
};

Message IN;
Message OUT;

void setup()
{
  softSerial.begin(4800);
  Serial.begin(9600);
  
}

void loop(){
  delay(1000);
  if(newdata == true){
  askSensor();
  }
  softSerial.listen();
  int readStatus = getDataFromStream(softSerial);
  if(readStatus == 0){
    if(IN.message.request == 100){
     Serial.print("Found sensor at address  : ");
      Serial.println(IN.message.body);
     Serial.println("----------------------");

      foundSensor = true;
      }
      newdata = false;
    }
  int buttonState = digitalRead(PIN_BUTTON);
      if(buttonState && foundSensor == true){
      
      OUT.message.request = senddatatosensor;
      OUT.message.codesensor = codesOfSensors[0];
      OUT.message.body = 102;
      OUT.message.checksum = OUT.message.request +  OUT.message.codesensor + OUT.message.body;
      writeDataToStream(softSerial);
        
      }
}
  void askSensor(){
   OUT.message.request = requestasksensor;
  OUT.message.codesensor = codesOfSensors[0];
  OUT.message.body = nodeaddress; 
  OUT.message.checksum = OUT.message.request+   OUT.message.codesensor + OUT.message.body;
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
    
  if(IN.message.request + IN.message.codesensor + IN.message.body  == IN.message.checksum) {
    return 0;
  }
  else return -1;
}

   
 
