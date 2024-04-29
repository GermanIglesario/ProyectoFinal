#include <Arduino.h>
#include <GCodeParser.h>
#include <Stepper.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <SD.h>

File archivo;

LiquidCrystal LCD(12,11,10,9,8,7); // Pines del LCD 20x4

void setup() {

  SD.begin(4); // Entre los parentesis va el pin CS al que este conectado el arduino
  Timer1.attachInterrupt(timer);
  Timer1.initialize(1000);
  LCD.begin(20,4);

}

void loop() {
  
  archivo = SD.open("nombre_archivo.txt"); // Se pasa el archivo al creado dentro del programa para poder trabajarlo
}

void timer(){

}
