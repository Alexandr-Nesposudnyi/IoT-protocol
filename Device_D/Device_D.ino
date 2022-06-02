//Device D
#include <SoftwareSerial.h>
SoftwareSerial softSerial(16,17);

bool newdata = true;
byte nodeaddress = 104;

int codesuccess = 100;
int requestasksensor = 77;
int senddatatosensor = 78;
bool foundSensor = false;

int valor_pot = 0;
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
  Serial.begin(9600);
  pinMode(A0, INPUT);

  
}
void loop(){
  delay(600);
   if(newdata == true){
  askSensor();
     delay(600);
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
  
  if(foundSensor == true){
    valor_pot = analogRead(A0);
    OUT.message.request = senddatatosensor;
    OUT.message.codesensor = valor_pot;
    OUT.message.body = IN.message.body;
    OUT.message.checksum = OUT.message.request +  OUT.message.codesensor + OUT.message.body;
     writeDataToStream(softSerial);
    delay(300);

  }
  }


 void askSensor(){
   OUT.message.request = requestasksensor;
  OUT.message.codesensor = codesOfSensors[2];
  OUT.message.body = nodeaddress; 
  OUT.message.checksum = OUT.message.request+OUT.message.codesensor+OUT.message.body;
 writeDataToStream(softSerial);
   //Serial.println("SendConfirmation");
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
