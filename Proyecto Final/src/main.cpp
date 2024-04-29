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

int bytesTotales = 0;
int ultimaPosicionSD = 0;
String cadena = "";
float coordenadas[2];

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
  interpretacion_SD();
  movimiento_PaP();

}

//--------------------------------------Funciones--------------------------------------

void lectura_SD(){

  archivo = SD.open("nombre_archivo.txt", FILE_READ); // Se pasa el archivo al creado dentro del programa para poder trabajarlo
  bytesTotales = archivo.size();

  if(archivo){
    if(ultimaPosicionSD <= bytesTotales){
      archivo.seek(ultimaPosicionSD);
    }
    while(archivo.available()){
      char caracter = archivo.read();
      if(caracter == 10){
        break;
      }
      cadena += caracter;
      ultimaPosicionSD = archivo.position();
    }
  }

  archivo.close();

}

void interpretacion_SD(){

}

void movimiento_PaP(){

}

//--------------------------------------Timer--------------------------------------

void timer(){

}
