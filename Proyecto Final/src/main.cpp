// El programa mas falopa que vas a ver en tu vida, con muy probablemente mas de 2000 lineas de codigo de pura
// mariguana de una calidad inigualable. VLLC!

#include <Arduino.h>
#include <string.h>
#include <GCodeParser.h>
#include <Stepper.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>

//--------------------------------------Inicializaciones de librerias--------------------------------------

Stepper myStepper(pasosPorRevolucion, 8, 9, 10, 11);

LiquidCrystal LCD(12,11,10,9,8,7); // Pines del LCD 20x4

//--------------------------------------Declaraciones e inicializaciones de variables--------------------------------------

String cadena = ""; // Concatenación?
float coordenadas[2]; // Del punto a grabar
const int pasosPorRevolucion = 200; // Segun los grados
int RPM = 60; // Revoluciones por minuto del motor paso a paso
int clock = 2; // Señal A del Encoder
int DT = 3; // Señal B del Encoder
int posicionAntes; // Variable Encoder
int posicionAhora; // Variable Encoder
int pulsEncoder = 0; // Pulsador Encoder
int estadoSwitch = 0;
int e = 1; // Variable de unica vez, pantalla de inicio

//--------------------------------------Setup--------------------------------------

void setup() {

  attachInterrupt(digitalPinToInterrupt(clock), encoder, LOW); // Interrupción externa
  Timer1.attachInterrupt(timer); // Timer
  Timer1.initialize(1000);
  LCD.begin(20,4); // LCD
  pinMode(clock, INPUT); // Encoder
  pinMode(DT, INPUT);
  pinMode(pulsEncoder, INPUT);
  posicionAntes = digitalRead(clock);

}

//--------------------------------------Loop--------------------------------------

void loop() {

  //-----------------------------------Pantalla-----------------------------------
  
  switch (estadoSwitch){ // Switch del menu visual

  case 0: // Inicio del programa

    if(e){
      LCD.setCursor(0,1);
      LCD.print("Grabadora Laser CNC");
      LCD.setCursor(1,3);
      LCD.print("Fefi's production");

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

      delay(2000);
    
      LCD.clear();
      estadoSwitch = 1;
    }

    estadoSwitch = 1;

    break;

  case 1: // Menu principal

    e = 0;
    delay(1000);
    LCD.setCursor(0,0);
    LCD.print("T:");
    //
    // Pantalla principal (pendiente)
    //
    while(digitalRead(pulsEncoder) == LOW){
      estadoSwitch = 1;
      delay(1);
      if(digitalRead(pulsEncoder) == HIGH){
        estadoSwitch = 2;
      }
    }

    break;
  
  case 2: // Opciones generales


    LCD.setCursor(20,0);
    LCD.cursor();
    
    break;
  }
  // Habra tantos casos como pantallas de opciones haya, estas pantallas se generan por cualquier cambio hecho
  // Los casos tienen que poder repetirse unicamente desde el 1 hasta el numero que se llegue
  // El caso 0 es unicamente el comienzo visual del programa

  //-----------------------------------Llamados-----------------------------------
  
  lectura();
  interpretacion();
  movimiento_PaP();

}

//--------------------------------------Funciones--------------------------------------

void lectura(){



}

void interpretacion(){

}

void movimiento_PaP(){

  myStepper.setSpeed(RPM);


}

//--------------------------------------Encoder--------------------------------------

void encoder(){

  if(digitalRead(DT) == HIGH){
    estadoSwitch += 1;
  }
  else{
    estadoSwitch -= 1;
  }

}

//--------------------------------------Timer--------------------------------------

void timer(){

}
