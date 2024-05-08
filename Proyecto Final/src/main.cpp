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
int estadoSwitch = 0; // Estado del switch segun el encoder
int estadoPrevioSwitch; // Estado previo del switch segun el encoder
int e = 1; // Variable de unica vez, pantalla de inicio
int a1 = 1; // Variable de unica vez, limpiar la pantalla
int a2 = 1; // Variable de unica vez, limpiar la pantalla

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

      delay(3000);

      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("Creada por:");
      LCD.setCursor(0,1);
      LCD.print("-Barcia");
      LCD.setCursor(0,2);
      LCD.print("-Iglesario");
      LCD.setCursor(0,3);
      LCD.print("-Lentini");

      delay(3000);
    
      LCD.clear();
      estadoSwitch = 1;
    }

    estadoSwitch = 1;

    break;

  case 1: // Menu principal

    if(estadoPrevioSwitch == 2){
      estadoSwitch = 2;
      break;
    }
    e = 0;
    if(a1){
      LCD.clear();
      a1 = 0;
      a2 = 1;
    }
    LCD.noCursor();
    delay(1000);
    LCD.setCursor(0,0);
    LCD.print("T:");
    //
    // Pantalla principal (pendiente)
    //
    while(1){ // Función que utiliza el encoder
      estadoSwitch = 1;
      if(digitalRead(pulsEncoder) == HIGH){
        estadoSwitch = 2;
        break;
      }
    }

    break;
  
  case 2: // Opciones generales

    if(a2){
      LCD.clear();
      a1 = 1;
      a2 = 0;
    }
    LCD.setCursor(0,0);
    LCD.print("- Volver al menu");
    LCD.setCursor(0,1);
    LCD.print("- Empezar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,3);
    LCD.print("- Terminar grabado");

    LCD.setCursor(16,0);
    LCD.cursor();
    estadoPrevioSwitch = 2;

    while(1){ // Función "volver al menu"
      if(digitalRead(pulsEncoder) == HIGH){ 
        estadoSwitch = 1;
        break;
      }
    }

    break;

  case 3:

    if(a1){
      LCD.clear();
      a1 = 0;
      a2 = 1;
    }
    LCD.setCursor(0,0);
    LCD.print("- Volver al menu");
    LCD.setCursor(0,1);
    LCD.print("- Empezar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,3);
    LCD.print("- Terminar grabado");

    LCD.setCursor(17,1);
    LCD.cursor();

    while(1){ // Función para selec. "empezar grabado"
      if(digitalRead(pulsEncoder) == HIGH){
        estadoSwitch = NULL; // Falta crear la función para empezar el grabado
        break;
      }
    }

    break;
  
  case 4:

    if(a2){
      LCD.clear();
      a1 = 1;
      a2 = 0;
    }
    LCD.setCursor(0,0);
    LCD.print("- Volver al menu");
    LCD.setCursor(0,1);
    LCD.print("- Empezar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,3);
    LCD.print("- Terminar grabado");

    LCD.setCursor(16,2);
    LCD.cursor();

    while(1){ // Función para selec. "pausar grabado"
      if(digitalRead(pulsEncoder) == HIGH){
        estadoSwitch = NULL; // Falta crear la función para pausar el grabado
        break;
      }
    }

    break;
  
  case 5:

    if(estadoPrevioSwitch == 6){
      estadoSwitch = 10;
      break;
    }
    if(a1){
      LCD.clear();
      a1 = 0;
      a2 = 1;
    }
    LCD.setCursor(0,0);
    LCD.print("- Volver al menu");
    LCD.setCursor(0,1);
    LCD.print("- Empezar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,3);
    LCD.print("- Terminar grabado");

    LCD.setCursor(18,3);
    LCD.cursor();

    while(1){ // Función para selec. "terminar grabado"
      if(digitalRead(pulsEncoder) == HIGH){
        estadoSwitch = NULL; // Falta crear la función para terminar el grabado
        break;
      }
    }

    break;
  
  case 6:

    if(a2){
      LCD.clear();
      a1 = 1;
      a2 = 0;
    }
    
    LCD.setCursor(0,0);
    LCD.print("- Empezar grabado");
    LCD.setCursor(0,1);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Terminar grabado");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(19,3);
    LCD.cursor();
    estadoPrevioSwitch = 6;

    while(1){ // Función para selec. "info del producto"
      if(digitalRead(pulsEncoder) == HIGH){
        estadoSwitch = NULL; // Falta crear la función para ver la información del producto
        break;
      }
    }

    break;

  case 7:

    if(estadoPrevioSwitch == 6){
      estadoSwitch = 6;
      break;
    }
    if(estadoPrevioSwitch == 8){
      estadoSwitch = 2;
      break;
    }

    break;

  case 8:

    if(a1){
      LCD.clear();
      a1 = 0;
      a2 = 1;
    }
    LCD.setCursor(0,0);
    LCD.print("- Empezar grabado");
    LCD.setCursor(0,1);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Terminar grabado");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(17,0);
    LCD.cursor();
    estadoPrevioSwitch = 8;

    while(1){ // Función para selec. "empezar grabado"
      if(digitalRead(pulsEncoder) == HIGH){
        estadoSwitch = NULL; // Falta crear la función para empezar el grabado
        break;
      }
    }

    break;

  case 9:

    if(a2){
      LCD.clear();
      a1 = 1;
      a2 = 0;
    }
    
    LCD.setCursor(0,0);
    LCD.print("- Empezar grabado");
    LCD.setCursor(0,1);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Terminar grabado");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(16,1);
    LCD.cursor();

    while(1){ // Función para selec. "pausar grabado"
      if(digitalRead(pulsEncoder) == HIGH){
        estadoSwitch = NULL; // Falta crear la función para pausar el grabado
        break;
      }
    }

    break;

  case 10:

    if(a1){
      LCD.clear();
      a1 = 0;
      a2 = 1;
    }
    
    LCD.setCursor(0,0);
    LCD.print("- Empezar grabado");
    LCD.setCursor(0,1);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Terminar grabado");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(18,2);
    LCD.cursor();
    estadoPrevioSwitch = 10;

    while(1){ // Función para selec. "terminar grabado"
      if(digitalRead(pulsEncoder) == HIGH){
        estadoSwitch = NULL; // Falta crear la función para terminar el grabado
        break;
      }
    }

    break;

  case 11:

    if(estadoPrevioSwitch == 10){
      estadoSwitch = 6;
    }

    break;
  
  }

  // Habra tantos casos como pantallas de opciones haya, estas pantallas se generan por cualquier cambio hecho
  // Los casos tienen que poder repetirse unicamente desde el 1 hasta el numero que se llegue
  // El caso 0 es unicamente el comienzo visual del programa

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
