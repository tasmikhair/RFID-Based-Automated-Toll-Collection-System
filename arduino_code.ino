#include <BitBool.h>
#include <OnewireKeypad.h>

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include<LiquidCrystal_I2C.h>

#define SS_PIN 10 //MOSI of RFID 
#define RST_PIN 9 //RST of RFID 
#define LED_DENIED_PIN 7
#define LED_ACCESS_PIN 6
#define sensorPin1 A2
#define sensorPin2 A3
#define buzzer 6

int senval1=0;
int senval2=0;
int card1Balance = 2000;
int card2Balance = 400;
int due1=0;
int due2=0;

String num1, num2, card, card2;
int a, b;
char Key;

bool recharge = true;

LiquidCrystal_I2C lcd(0x3F, 16, 2);
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
Servo myservo; //define servo name
int state=0;

void setup() 
{
  lcd.init();
  lcd.backlight();

  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(buzzer, OUTPUT);

  pinMode(LED_DENIED_PIN, OUTPUT);
  pinMode(LED_ACCESS_PIN, OUTPUT);

  myservo.attach(3); //servo pin
  myservo.write(90); //servo start position

  Serial.println();
  lcd.setCursor(1, 0);
  lcd.print("Automatic Toll");
  lcd.setCursor(2, 1);
  lcd.print("Tax System");
  delay(2000);
}
void loop() 
{
  // lcd.clear();
  // lcd.setCursor(0, 0);
  // lcd.print("   Welcome!!");
  // delay(1000);
  // lcd.clear();
  // if(recharge==0)
  // {
  //   reCharge();
  // }
  // else
  // {
  sensorRead();
  rfid();
  if(senval1==0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Vehicle Detected");
    myservo.write(0);
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan your card");
    delay(1000);
  }
  else if(senval2==0 && state==1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Have a safe");
    lcd.setCursor(0, 1);
    lcd.print("journey");
    //buz code
    myservo.write(90);
    delay(4000);
    myservo.write(0);
    delay(1000);
    lcd.clear();
    state=0;
  }
  else if(senval2==0 && state==0){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("      NOT");
    lcd.setCursor(0, 1);
    lcd.print("    ALLOWED");
    delay(1000);
    lcd.clear();
    state=0;
  }
  //}
}
void sensorRead()
{
  senval1 = digitalRead(sensorPin1);
  senval2 = digitalRead(sensorPin2);
}

void rfid()
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
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
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "1C BA 59 4A") //change here the UID of the card/cards that you want to give access
  {
      digitalWrite(LED_ACCESS_PIN, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Authorized");
      lcd.setCursor(0, 1);
      lcd.print("Vehicle");
      delay(1000);
      lcd.clear();
    if(card1Balance>=500)
    {
      card1Balance = card1Balance-500;
      lcdPrint();
      lcd.setCursor(9, 1);
      lcd.print(card1Balance);
      //delay(1000);
      //lcd.clear();
      state=1;
    }
    else
    {
      card = content.substring(1);
      LcdPrint();
      //lcd.setCursor(9, 1);
      due1+=500;
      lcd.print(card1Balance);
      lcd.print(" Tk");
      // delay(1000);
      // lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Due: ");
      lcd.print(due1);
      lcd.print(" Tk");
      //delay(2000);
     // lcd.clear();
      state = 1;
    }
  }
  else if(content.substring(1) == "33 9E 20 0F")
  {
    digitalWrite(LED_ACCESS_PIN, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Authorized");
      lcd.setCursor(0, 1);
      lcd.print("Vehicle");
      delay(1000);
      lcd.clear();
    if(card2Balance>=500)
    {
      card2Balance = card2Balance-500;
      lcdPrint();
      due2+=500;
      lcd.setCursor(9, 1);
      lcd.print(card2Balance);
      //delay(1000);
      //lcd.clear();
      state=1;
    }
    else
    {
      card = content.substring(1);
      due2+=500;
      LcdPrint();
      //lcd.setCursor(9, 1);
      //ekhaneeeeeeeeee
      lcd.print(card2Balance);
      lcd.print(" Tk");
      // delay(1000);
      // lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Due: ");
      lcd.print(due2);
      lcd.print(" Tk");
     // delay(1000);
      //lcd.clear();
      state = 1;
    }
  }

 else{
    Serial.println(" Access denied");
    digitalWrite(LED_DENIED_PIN, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access denied..");
    delay(1000);
    lcd.clear();
    state=0;
  }
}


void lcdPrint()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Successfully");
  lcd.setCursor(0, 1);
  lcd.print(" paid your bill");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Your Remaining");
  lcd.setCursor(0, 1);
  lcd.print("Balance: ");
}

void LcdPrint()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  Your balance");
  lcd.setCursor(0, 1);
  lcd.print(" is insufficent");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Due Added!!");
  lcd.setCursor(0, 1);
  lcd.print("to your account");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Balance: ");
}