//Author JihanAlam_ID:16301199_Theory_Section_01_Lab_Section 01
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define SERVO_PIN 8
#define TIMER_PIN 5
#define GREEN_PIN 4
#define RED_PIN 3


MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo_1;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

boolean park_space[]={0,0,0,0,0,0,0};
int time_table[]={0,0,0,0,0,0,0};
int timer=0;
int space=6;

void setup() 
{
  servo_1.attach(SERVO_PIN);
  lcd.begin(16,2);
  update_display();
  pinMode(TIMER_PIN, INPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  Serial.begin(9600);   
  SPI.begin();          
  mfrc522.PCD_Init();   
  Serial.println("Put your card to the reader...");
  Serial.println();
}

void loop() 
{ 
  int val = digitalRead(TIMER_PIN);
  if(val==HIGH){
    timer++;
    Serial.println(timer);
    delay(500);
    }
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  
  if (content.substring(1) == "31 DF 39 20"){
    add_or_removecar(1);
  }
  else if (content.substring(1) == "31 6B 83 20"){
    add_or_removecar(2);
  }
  else if (content.substring(1) == "94 83 1C BA"){
    add_or_removecar(3);
  }
  else if (content.substring(1) == "41 0D FC 20"){
    add_or_removecar(4);
  }
  else if (content.substring(1) == "31 F2 7C 20"){
    add_or_removecar(5);
  }
  else if (content.substring(1) == "41 15 EE 20"){
    add_or_removecar(6);
  }
  else{
    Serial.println("invalid tag");
  }
}

void add_or_removecar(int index){
  if(park_space[index]==false){
      park_space[index]=true;
      time_table[index]=timer;
      space--;
      }
    else if(park_space[index]==true){
      park_space[index]=false;
      calculate_bill(index);
      time_table[index]=0;
      space++;
      }
      open_gate();
      delay(3000);
      close_gate();
      delay(2000);
      update_display();
}

void calculate_bill(int index){ //test
    int bill_per_hour=20;
    int parking_time=(timer-time_table[index]);
    int total_bill=(parking_time*bill_per_hour);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Parked for:"+String(parking_time)+"h");
    lcd.setCursor(0,1);
    lcd.print("Parking Bill:"+String(total_bill));
    delay(3000);
}

void open_gate(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Opening Gate....");  
  servo_1.write(180);
}

void close_gate(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Closing Gate...."); 
  servo_1.write(90);
}

void update_display(){
  if(space==0){
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(GREEN_PIN, LOW);
    }
    else{
    digitalWrite(GREEN_PIN, HIGH);
    digitalWrite(RED_PIN, LOW);
    }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Space left: "+ String(space));
}
