/*
    CSBomb this arduino program emulates a bomb device for airsoft games.
    Copyright (C) 2016  Kaio Marcondes - kaiomm@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/* Pin definitions */
#define buzzPin 9
#define plantPin 14
#define defusePin 15
#define LEDred 13
#define LEDgreen 12
#define lcdRS 4
#define lcdEN 2
#define lcdD4 5
#define lcdD5 6
#define lcdD6 7
#define lcdD7 8
#define lcdContrast 3
/* Time definitions */
#define countdownStandardValue 45
#define defuseStandardValue 10
#define plantStandardValue 2
/* Display definitions */
#define contrastStandardValue 100
/* Sound definitions */
#define countdownFrequency 4400
#define buttonFrequency 2200
#define explosionFrequency 4400
/* Delays definitions*/
#define delayBetweenInputs 200
#define delayBetweenConfirms 500
#define delayBetweenRandomNumbers 200
#define delayWelcome 5000
#define dramaticPause 2000
/* Parameters definitions */
#define showCountdown false //Enable to show countdown timer while bomb planted
#define useLcdPwm false     //Enable to use programmable contrast on display via PWM

#include <LiquidCrystal.h>

int plantButtonState = 0;
int plantState=0;
int defuseButtonState=0;
int defusingState=0;

int countdown=0;
long int count;
long int current;

long int beepPrevious;
long int beepInterval;
long int defusePrevious;
long int defuseMillis;
long int defuseFirst;
long int defuseNumbersInterval;
long int plantInterval;
int gameover=0;
int score_ct=0;
int score_t=0;

int contrastValue=contrastStandardValue;
int countdownValue=countdownStandardValue;
int defuseValue=defuseStandardValue;
int plantValue=plantStandardValue;

LiquidCrystal lcd(lcdRS, lcdEN, lcdD4, lcdD5, lcdD6, lcdD7);
void setup() {
  pinMode(buzzPin, OUTPUT);
  pinMode(LEDred, OUTPUT);
  pinMode(LEDgreen, OUTPUT);
  pinMode(plantPin, INPUT);
  pinMode(defusePin, INPUT);
  
  digitalWrite(LEDred,LOW);
  digitalWrite(LEDgreen,LOW);

  analogWrite(lcdContrast,contrastValue);
  
  lcd.begin(16,2);
  lcd.noAutoscroll();
//Splash screen
  lcd.setCursor(0,0);
  lcd.print("kaiomm's CSBomb");
  lcd.setCursor(0,1);
  lcd.print("   version 3   ");
  delay(delayWelcome);
  lcd.clear();
//Time/display configurations
  lcd.setCursor(0,0);
  lcd.print("CONFIG ");
  #if useLcdPwm
    lcd.setCursor(0,1);
    lcd.print("CONTRAST ");
    while(!(digitalRead(plantPin)==HIGH && digitalRead(defusePin)==HIGH)){
      lcd.setCursor(9,1);
      if(digitalRead(plantPin)==HIGH)
        contrastValue+=1;
      if(digitalRead(defusePin)==HIGH)
        contrastValue-=1;
      if(contrastValue<10)
        lcd.print(" ");
      if(contrastValue<100)
        lcd.print(" ");
      lcd.print(contrastValue);
      analogWrite(lcdContrast,contrastValue);
      delay(delayBetweenInputs);
    }
    lcd.setCursor(0,1);
    lcd.print("                ");
  #endif
  lcd.setCursor(0,1);
  lcd.print("BOMB ");
  delay(delayBetweenConfirms);
  while(!(digitalRead(plantPin)==HIGH && digitalRead(defusePin)==HIGH)){
    lcd.setCursor(5,1);
    if(digitalRead(plantPin)==HIGH)
      countdownValue+=1;
    if(digitalRead(defusePin)==HIGH)
      if(countdownValue>1)
        countdownValue-=1;
    lcd.print(countdownValue);
    if(countdownValue<10)
      lcd.print(" ");
    if(countdown<100)
      lcd.print(" ");
    delay(delayBetweenInputs);
  }
  lcd.setCursor(7,0);
  lcd.print("DEFUSE ");
  delay(delayBetweenConfirms);
  while(!(digitalRead(plantPin)==HIGH && digitalRead(defusePin)==HIGH)){
    lcd.setCursor(14,0);
    if(digitalRead(plantPin)==HIGH)
      defuseValue+=1;
    if(digitalRead(defusePin)==HIGH)
      if(defuseValue>1)
        defuseValue-=1;
    lcd.print(defuseValue);
    if(defuseValue<10)
      lcd.print(" ");
    delay(delayBetweenInputs);
  }
  lcd.setCursor(8,1);
  lcd.print("PLANT ");
  delay(delayBetweenConfirms);
  while(!(digitalRead(plantPin)==HIGH && digitalRead(defusePin)==HIGH)){
    lcd.setCursor(14,1);
    if(digitalRead(plantPin)==HIGH)
      plantValue+=1;
    if(digitalRead(defusePin)==HIGH)
      if(plantValue>1)
        plantValue-=1;
    lcd.print(plantValue);
    if(plantValue<10)
      lcd.print(" ");
    delay(delayBetweenInputs);
  }
  delay(delayBetweenConfirms);

  defuseNumbersInterval=defuseValue*1000/7;
  plantInterval=plantValue*1000/8;

  randomSeed(analogRead(A0));
}

void loop() {
  if(!gameover){
    plantButtonState = digitalRead(plantPin);
    if(!plantState){  //plantState FALSE
      if(plantButtonState==HIGH){ //plantState FALSE && plantButtonState TRUE
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0, 0);
        lcd.print(" PLANTING BOMB! ");
        delay(plantInterval);
        tone(buzzPin, buttonFrequency, 50);
        plantButtonState = digitalRead(plantPin);
        if(plantButtonState==HIGH){
          lcd.setCursor(0,1);
          lcd.print("7 ");
          delay(plantInterval);
          tone(buzzPin, buttonFrequency, 50);
          plantButtonState = digitalRead(plantPin);
          if(plantButtonState==HIGH){
            lcd.print("3 ");
            delay(plantInterval);
            tone(buzzPin, buttonFrequency, 50);
            plantButtonState = digitalRead(plantPin);
            if(plantButtonState==HIGH){
              lcd.print("5 ");
              delay(plantInterval);
              tone(buzzPin, buttonFrequency, 50);
              plantButtonState = digitalRead(plantPin);
              if(plantButtonState==HIGH){
                lcd.print("5 ");
                delay(plantInterval);
                tone(buzzPin, buttonFrequency, 50);
                plantButtonState = digitalRead(plantPin);
                if(plantButtonState==HIGH){
                  lcd.print("6 ");
                  delay(plantInterval);
                  tone(buzzPin, buttonFrequency, 50);
                  plantButtonState = digitalRead(plantPin);
                  if(plantButtonState==HIGH){
                    lcd.print("0 ");
                    delay(plantInterval);
                    tone(buzzPin, buttonFrequency, 50);
                    plantButtonState = digitalRead(plantPin);
                    if(plantButtonState==HIGH){
                      lcd.print("8 ");
                      delay(plantInterval);
                      tone(buzzPin, buttonFrequency, 50);
                      plantButtonState = digitalRead(plantPin);
                      if(plantButtonState==HIGH){
                        plantState=1;
                        countdown=countdownValue;
                        count=0;
                        beepPrevious=0;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      else{ //plantState FALSE && plantButtonState LOW
        lcd.setCursor(0,0);
        lcd.print("   BOMB READY   ");
        lcd.setCursor(0,1);
        lcd.print("    T vs CT     ");
        lcd.setCursor(0,1);
        lcd.print(score_t);
        lcd.setCursor(14,1);
        lcd.print(score_ct);
      }
    }
    else{ //plantState TRUE
      current=millis();
      if(current-count>=1000){
        count=current;
        countdown-=1;
      }
      beepInterval=100+countdown*(900/countdownValue);
      if (current-beepPrevious>=beepInterval){
        beepPrevious=current;
        tone(buzzPin,countdownFrequency,100);
      }
      defuseButtonState= digitalRead(defusePin);
      if(defuseButtonState==LOW){
        defusingState=0;
        defuseMillis=0;
      }
      if(defuseButtonState==HIGH && defusePrevious==LOW){
        defusingState=1;
        defuseFirst=current;
      }
      if(defuseButtonState==HIGH && defusePrevious==HIGH){
        defusingState=1;
        defuseMillis=current-defuseFirst;
      }
      defusePrevious=defuseButtonState;
      lcd.clear();
      lcd.setCursor(0,0);
      if(!defusingState){
        #if !showCountdown
          lcd.print("  ");
        #endif
        lcd.print("BOMB PLANTED");
        lcd.setCursor(0,1);
        lcd.print(" * * * * * * * ");      
      }
      else{
        #if !showCountdown
          lcd.print(" ");
        #endif
        lcd.print("DEFUSING BOMB");
        lcd.setCursor(1,1);
        if (defuseMillis>=defuseNumbersInterval*1)
          lcd.print("7");
        else if((millis()%delayBetweenRandomNumbers))
          lcd.print((int)random(10));
        lcd.print(" ");
        if (defuseMillis>=defuseNumbersInterval*2)
          lcd.print("3");
        else if((millis()%delayBetweenRandomNumbers))
          lcd.print((int)random(10));
        lcd.print(" ");
        if (defuseMillis>=defuseNumbersInterval*3)
          lcd.print("5");
        else if((millis()%delayBetweenRandomNumbers))
          lcd.print((int)random(10));
        lcd.print(" ");
        if (defuseMillis>=defuseNumbersInterval*4)
          lcd.print("5");
        else if((millis()%delayBetweenRandomNumbers))
          lcd.print((int)random(10));
        lcd.print(" ");
        if (defuseMillis>=defuseNumbersInterval*5)
          lcd.print("6");
        else if((millis()%delayBetweenRandomNumbers))
          lcd.print((int)random(10));
        lcd.print(" ");
        if (defuseMillis>=defuseNumbersInterval*6)
          lcd.print("0");
        else if((millis()%delayBetweenRandomNumbers))
          lcd.print((int)random(10));
        lcd.print(" ");
        if (defuseMillis>=defuseNumbersInterval*7)
          lcd.print("8");
        else if((millis()%delayBetweenRandomNumbers))
          lcd.print((int)random(10));
        lcd.print(" ");
      }
      #if showCountdown
        lcd.setCursor(13,0);
        if(countdown<10)
          lcd.print(" "); 
        if(countdown<100)
          lcd.print(" "); 
        lcd.print(countdown);
      #endif
      if (countdown==0){
        gameover=1;
        delay(dramaticPause);
        digitalWrite(LEDred, HIGH);
        tone(buzzPin, 4400, 10000);
        lcd.setCursor(0,0);
        lcd.print(" BOMB EXPLODED  ");
        lcd.setCursor(0,1);
        lcd.print(" TERRORIST WIN ");
        score_t+=1;        
      }
      if(defuseMillis>=defuseValue*1000){
        gameover=1;
        delay(dramaticPause);
        digitalWrite(LEDgreen,HIGH);
        lcd.setCursor(0,0);
        lcd.print("  BOMB DEFUSED  ");
        lcd.setCursor(0,1);
        lcd.print("   CT WIN  T=");
        lcd.print(countdown);
        score_ct+=1;
      }
    }
  }
  else{ //gameover
    plantState=0;
    do{
      gameover=0;
      digitalWrite(LEDred,LOW);
      digitalWrite(LEDgreen,LOW);
    } while(!(digitalRead(plantPin)==HIGH && digitalRead(defusePin)==HIGH));
  }
}
