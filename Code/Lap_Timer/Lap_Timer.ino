#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Ultrasonic.h>
Ultrasonic ultrasonic(3, 4);
int distance;
LiquidCrystal_I2C lcd(0x27, 20,4);
int m = 0;
int s = 0;
int cs = 0;

#define ACTUADOR 6
#define RST 7

bool rst = 0;
bool ant_rst = 0;
bool actuador = 0;
bool ant_actuador = 0;


long now = millis();
long ant = 0;
long ant_start = 0;
long ant_ultra = 0;

//Switch
#define ESPERA 0
#define CORRIENDO 1
#define PARADO 2
#define RESET 3
int estado = 0;

//#define SERIAL


void setup() {
  Serial.begin(115200);
  #ifdef SERIAL
    Serial.begin(115200);
    Serial.println("\nBienvenido al Lap Timer");
    Serial.println("Pulsa Start/Stop Para iniciar y Reset para reiniciar ☻");
  #endif
  lcd.begin();
  pinMode(ACTUADOR, INPUT_PULLUP);
  pinMode(RST, INPUT_PULLUP);
  //pinMode(D0, OUTPUT);
  //digitalWrite(D0, LOW);
  estado = RESET;
  lcd.home();
  lcd.print("   Lap Timer");
}

void loop() {
    rst = !digitalRead(RST);
    actuador = !digitalRead(ACTUADOR);
    now = millis();
    if((now-ant_ultra)>20){
        ant_ultra = now;
        distance = ultrasonic.read();
        Serial.print(distance);Serial.println("cm");
    }
    
    
    if(distance<20){
        actuador = HIGH;
    }else{
        actuador = LOW;
    }

    switch(estado){
      case ESPERA:
          if((actuador == HIGH)&&(ant_actuador == LOW)){
              estado = CORRIENDO;
              ant_start = now;
          }
          ant_actuador = actuador;
          break;
      case CORRIENDO:
          if((now-ant_start)>2000){
              if((actuador == HIGH)&&(ant_actuador == LOW)){
                  estado = PARADO;
              }
          }
          ant_actuador = actuador;
          contador();
          break;
      case PARADO:
          if((rst == HIGH)&&(ant_rst == LOW)){
              estado = RESET;
          }
          ant_rst = rst;
          break;
      case RESET:
          m = 0;
          s = 0;
          cs = 0;
          LCD_clear();
          #ifdef SERIAL
            Serial.println("00:00:000");
          #endif
          estado = ESPERA;
          break;
    }
      
}


void contador(){
  if((now-ant)>=10){
      ant=now;
      cs++;
      LCD_cen();
      if(cs >= 100){
          cs = 0;
          s++;
          LCD_seg();
      }
      if(s >= 60){
          s = 0;
          m++;
          LCD_min();
      }
      #ifdef SERIAL
        Serial.print(m);Serial.print(":");Serial.print(s);Serial.print(":");Serial.println(cs);
      #endif
  }
}

void LCD_min(){
    lcd.setCursor(0,1);
    if(m < 10){
      lcd.print("0");
    }
    lcd.print(m);
    lcd.print(":");
}
void LCD_seg(){
    lcd.setCursor(3,1);
    if(s < 10){
      lcd.print("0");
    }
    lcd.print(s);
    lcd.print(":");
}
void LCD_cen(){
    lcd.setCursor(6,1);
    lcd.print(cs);
}
void LCD_clear(){
    lcd.clear();
    lcd.home();
    lcd.print("   Lap Timer");
    lcd.setCursor(0,1);
    lcd.print("00:00:000");
}
