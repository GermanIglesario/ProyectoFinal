#include <Arduino.h>
#include <string.h>
#include <GCodeParser.h>
#include <Stepper.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <SD.h>

//--------------------------------------Inicializaciones de librerias--------------------------------------

File archivo;

Stepper myStepper(pasosPorRevolucion, 8, 9, 10, 11);

LiquidCrystal LCD(12,11,10,9,8,7); // Pines del LCD 20x4

//--------------------------------------Declaraciones e inicializaciones de variables--------------------------------------

int bytesTotales = 0; // Posicion en archivo SD
int ultimaPosicionSD = 0; // Backup
String cadena = ""; // Concatenación?
float coordenadas[2]; // Del punto a grabar
const int pasosPorRevolucion = 200; // Segun los grados
int RPM = 60; // Revoluciones por minuto del motor paso a paso
int clock = 2; // Clock de Encoder
int DT = 3; // ???
int estadoAntes; // Variable Encoder
int estadoActual; // Variable Encoder
int estadoSwitch = 0;
volatile int cont = 0;

//--------------------------------------Setup--------------------------------------

void setup() {

  SD.begin(4); // Entre los parentesis va el pin CS al que este conectado el arduino
  Timer1.attachInterrupt(timer);
  Timer1.initialize(1000);
  LCD.begin(20,4);
  pinMode(clock, INPUT);
  pinMode(DT, INPUT);
  estadoAntes = digitalRead(clock);

}

//--------------------------------------Loop--------------------------------------

void loop() {

  //-----------------------------------Encoder-----------------------------------



  //-----------------------------------Pantalla-----------------------------------
  
  switch (estadoSwitch){

  case 0:

    cont = 0;
    LCD.setCursor(0,0);
    LCD.print("Grabadora Laser CNC");

    delay(2000);

    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("Creada por:");
    LCD.setCursor(0,1);
    LCD.print("-Barcia");
    LCD.setCursor(0,2);
    LCD.print("-Iglesario");
    LCD.setCursor(0,3);
    LCD.print("-Lentini");
    

    break;

  case 1:

    break;
  }

  //-----------------------------------Llamados-----------------------------------
  
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

  myStepper.setSpeed(RPM);


}

//--------------------------------------Timer--------------------------------------

void timer(){

}
