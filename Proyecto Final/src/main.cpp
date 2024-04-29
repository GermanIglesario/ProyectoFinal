#include <Arduino.h>
#include <string.h>
#include <GCodeParser.h>
#include <Stepper.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <SD.h>

//--------------------------------------Inicializaciones de librerias--------------------------------------

File archivo;

LiquidCrystal LCD(12,11,10,9,8,7); // Pines del LCD 20x4

//--------------------------------------Declaraciones e inicializaciones de variables--------------------------------------

int ultimaPosicionSD = 0;

//--------------------------------------Setup--------------------------------------

void setup() {

  SD.begin(4); // Entre los parentesis va el pin CS al que este conectado el arduino
  Timer1.attachInterrupt(timer);
  Timer1.initialize(1000);
  LCD.begin(20,4);

}

//--------------------------------------Loop--------------------------------------

void loop() {
  
  lectura_SD();

}

//--------------------------------------Funciones--------------------------------------

char lectura_SD(){

  archivo = SD.open("nombre_archivo.txt", FILE_READ); // Se pasa el archivo al creado dentro del programa para poder trabajarlo


}

//--------------------------------------Timer--------------------------------------

void timer(){

}
