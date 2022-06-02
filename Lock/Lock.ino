#include <LiquidCrystal.h>   
#include <Keypad.h>
#include <SoftwareSerial.h>

SoftwareSerial softSerial(16,17);
int flood = 0;
bool clear  = false;
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

int a=0, b=0, c=0, d=0;// Integers that will store the password
int var=0; // Auxiliary variable for the switch
int C1=1,C2=2,C3=3,C4=4;//Setting the password
char f='*';  // Character to hide the original character
int times=0,incorrect=0; //Safety of only 3 attempts.
int notice=3; //Warning to know how many attempts it will take
const byte rows = 4; //number of queues.
const byte columns = 4; //number of columns.

bool lock = false;

char key[rows][columns] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte pinRows[rows] = {7, 6, 5, 4}; //declaration of the pins connected to the queues
byte pinColumn[columns] = {3, 2, 1, 0}; //declaration of the pins connected to the columns

Keypad keypad = Keypad( makeKeymap(key), pinRows, pinColumn, rows, columns );
LiquidCrystal lcd(8,9,10,11,12,13); //D7,D6,D4,E,RS
void setup(){
  lcd.begin(16,2); //LCD (16 COLUMNS and 2 ROWS)
  pinMode(A0,OUTPUT); //Green LED - Pass correct
  pinMode(A1,OUTPUT); //Red LED - Incorrect pass
 //Serial.begin(9600);
  softSerial.begin(4800);
  
}
  
void loop(){
  if(clear == false){
  char key = keypad.getKey();
  if (key){
   lcd.setCursor(6+var,1);
   lcd.print(key),lcd.setCursor(6+var,1),lcd.print(f);//print * character on lcd
   key=key-48; //conversion of char to integers
   var++; 
   
   switch(var){
    case 1:
    a=key; //store the first digit for the password
    break;
    case 2:
    b=key; 
    break;
    case 3:
    c=key; 
    break;
    case 4:
    d=key; 
   delay(100);
  if(a==C1 && b==C2 && c==C3 && d==C4){//if a,b,c,d are equal to the one declared above in the pass then
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Password");
    lcd.setCursor(5,1);
    lcd.print("Valid");
    digitalWrite(A0,HIGH);//Green LED lights up
   // SendConfirmation();
    clear = true;
    delay(1000); // stays on for 1 second
    lcd.clear();
    digitalWrite(A0,LOW);//Green LED turns off
    }
  else{
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Password");
    lcd.setCursor(4,1);
    lcd.print("Invalid");
    digitalWrite(A1,HIGH); //Red LED lights up
    delay(1000); // stays on for 1 second
    lcd.clear();
    digitalWrite(A1,LOW); 
    }
//------Security of the three faults-------------------//

  if(a==C1 && b==C2 && c==C3 && d==C4){
      times=0;//If it is correct, the variable times does not change
      notice=3;//And the warning variable remains at 3
    }
  else{
      times ++; //add 1 value to the variable times
      notice --; //decrease the warning variable by 1 value. until reaching 0
      lcd.setCursor(2,0);
      lcd.print("ATTEMPTS: "); 
      lcd.setCursor(13,0);
      lcd.print(notice); //print the number of remaining opportunities
      lcd.setCursor(2,1);
      lcd.print("OPPORTUNITY");
      delay(1000);
       if(notice==0){
          lcd.clear();
          lcd.setCursor(5,0);
          lcd.print("ALARM");
          lcd.setCursor(4,1);
          lcd.print("ACTIVATED");
        }
      delay(300);lcd.clear();
      }

  while(times>=3){ //when it reaches the wrong 3 times, the message will flash
      lcd.setCursor(1,0),lcd.print("Police Alert");
      lcd.setCursor(4,1),lcd.print("intruders");
      digitalWrite(A1,HIGH); // turn on the red LED
      delay(500);
      lcd.clear();
      digitalWrite(A1,LOW);
      delay(500);
  }

   var=0;
   lcd.clear();

  break;
  }

 }
 if(!key){lcd.setCursor(0,0),lcd.print("Enter Password");}//start message

  delay(2);
  }
  
  else{
   lcd.clear();
   lcd.print("Home is working!");
    delay(1000);
    SendConfirmation();
  }
}

void SendConfirmation(){
 
  OUT.message.request = 75 ;
  OUT.message.codesensor = 0;
  OUT.message.body = 110;
  OUT.message.checksum = OUT.message.request +   OUT.message.codesensor + OUT.message.body;
  writeDataToStream(softSerial);
  
}

int writeDataToStream(Stream &softSerial) {
  int count = 0;
  for (int i=0; i<sizeof(OUT); i++) {
    softSerial.write(OUT.MessageBytes[i]);
    count++;
  }
}
