// Device C

#include <SoftwareSerial.h>
SoftwareSerial softSerial(12,13);
#include<Servo.h>
Servo servo;

bool newdata = true;
byte nodeaddress = 103;
byte sensornode = 0;
int requestgiveSensor = 76;
byte  codesOfSensors[] = {1,2,3,4};
/*-----
1 - LED
2 - temp
3 - motor
------*/
const int  pinoServo = 9;

int serv ;
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
  softSerial.begin(4800);
  Serial.begin(9600);
    servo.attach(pinoServo);

}

void loop()
{
  delay(500);
  if(newdata == true){
     sendIformationofSensor();
    delay(1000);

  }
  delay(100);
 softSerial.listen();
  int readStatus = getDataFromStream(softSerial);
  if(readStatus == 0){
    if(IN.message.request == 100){
    Serial.println("OK");
    
   // ServoT();
    }
    if(IN.message.request == 78){
     Serial.println("Datta to sensor");
      Serial.println(IN.message.codesensor);
      serv = IN.message.codesensor;
    //  valor_pot = map(valor_pot,0,1023,0,180);
  serv = map(serv,0,1023,0,180);
      servo.write(serv);
      delay(15);
    }
     delay(200);

  }
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
    
  if(IN.message.request + IN.message.body + IN.message.codesensor == IN.message.checksum) {
    return 0;
  }
  else return -1;
}

void sendIformationofSensor(){
   OUT.message.request = requestgiveSensor;
   OUT.message.codesensor = codesOfSensors[2];
  OUT.message.body = nodeaddress;
  OUT.message.checksum = OUT.message.request + OUT.message.codesensor + OUT.message.body ;
  writeDataToStream(softSerial);
    
  
}
