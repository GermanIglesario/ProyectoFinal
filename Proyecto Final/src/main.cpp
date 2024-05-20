// El programa mas falopa que vas a ver en tu vida, con muy probablemente mas de 2000 lineas de codigo de pura
// mariguana de una calidad inigualable. VLLC!

// La maquina tiene una presición de 40 micrometros por paso (2 globulos blancos de grosor)

#include <Arduino.h>
#include <string.h>
#include <GCodeParser.h>
#include <Stepper.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <TimerOne.h>
#include <MsTimer2.h>
#include <Servo.h>
#include <SD.h>

////--------------------------------------Inicializaciones de librerias--------------------------------------

Stepper myStepper1(pasosPorRevolucion, 9, 10, 11, 12);
Stepper myStepper2(pasosPorRevolucion, 5, 6, 7, 8);


LiquidCrystal_I2C LCD(0x27, 20, 4); // Pines del LCD 20x4

////--------------------------------------Declaraciones e inicializaciones de variables--------------------------------------

//--------------------------------------Coordenadas e interpretación--------------------------------------

String cadena = ""; // Concatenación?
float coordenadas[2]; // Del punto a grabar
float coordenadasPrevias[2]; // Del punto anterior

//--------------------------------------PaP y relacionados--------------------------------------

const int finalX = A0;
const int finalY = A1;
const int pasosPorRevolucion = 200; // Segun los grados
int RPM = 60; // Revoluciones por minuto del motor paso a paso

//--------------------------------------Encoder y funcionamiento del menu--------------------------------------

int clk = 2; // Señal A del Encoder
int DT = 3; // Señal B del Encoder
int posicionAntes; // Variable Encoder
int posicionAhora; // Variable Encoder
int pulsEncoder = 4; // Pulsador Encoder
int estadoSwitch = 0; // Estado del switch segun el encoder
int estadoPrevioSwitch; // Estado previo del switch segun el encoder
bool deteccionCambioSwitch1; // Variable boleana que le dice que valor tomar a estadoSwitch
bool deteccionCambioSwitch2; // Variable boleana que le dice que valor tomar a estadoSwitch
int e = 1; // Variable de unica vez, pantalla de inicio
int a1 = 1; // Variable de unica vez, limpiar la pantalla
int a2 = 1; // Variable de unica vez, limpiar la pantalla

//--------------------------------------Menu Principal--------------------------------------

int temperatura; // Temperatura de la punta
int progreso; // Porcentaje del grabado (si puedo sacarlo XD)
int tiempo; // Tiempo que lleva el grabado

////--------------------------------------Prototipado Funciones--------------------------------------

void encoder();
void lectura_SD();
void interpretacion_SD();
void movimiento_PaP();
void retorno_Al_Home();

////--------------------------------------Setup--------------------------------------

void setup() {

  attachInterrupt(digitalPinToInterrupt(5), interrupcion_Externa, LOW); // Interrupción externa (falta definir PIN)
  MsTimer2::set(1, timer); // Interrupción cada 1ms
  MsTimer2::start(); // Inicio timer
  LCD.init(); // LCD
  LCD.backlight();
  LCD.clear();
  pinMode(clk, INPUT); // Encoder
  pinMode(DT, INPUT);
  pinMode(pulsEncoder, INPUT);
  posicionAntes = digitalRead(clk);
  myStepper1.setSpeed(50); // Stepper 1
  myStepper2.setSpeed(50); // Stepper 2
  pinMode(finalX, INPUT);
  pinMode(finalY, INPUT);

}

////--------------------------------------Loop--------------------------------------

void loop() {

  //-----------------------------------Pantalla-----------------------------------

  // Caso 0: Introducción
  // Caso 1: Menu Principal
  // Caso 2 al 11: Menu de Opciones
  // Caso 12 al 13: Opcion "Info del Producto"
  
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
      LCD.print("- Barcia");
      LCD.setCursor(0,2);
      LCD.print("- Iglesario");
      LCD.setCursor(0,3);
      LCD.print("- Lentini");

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
    delay(500);
    LCD.setCursor(0,0);
    LCD.print("T:");
    //
    // Pantalla principal (pendiente)
    //
    while(1){ // Permanencia en el menu
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
    LCD.print("*");
    estadoPrevioSwitch = 2;

    encoder();
    if(digitalRead(pulsEncoder) == HIGH){ // Función "volver al menu"
      estadoSwitch = 1;
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
    LCD.print("*");

    encoder();
    if(digitalRead(pulsEncoder) == HIGH){ // Función para selec. "empezar grabado"
      estadoSwitch = NULL;
      break;
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
    LCD.print("*");

    encoder();
    if(digitalRead(pulsEncoder) == HIGH){ // Función para selec. "pausar grabado"
      estadoSwitch = NULL;
      break;
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
    LCD.print("*");

    encoder();
    if(digitalRead(pulsEncoder) == HIGH){ // Función para selec. "terminar grabado"
      estadoSwitch = NULL;
      break;
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
    LCD.print("*");
    estadoPrevioSwitch = 6;

    encoder();
    if(digitalRead(pulsEncoder) == HIGH){ // Función para selec. "info del producto"
      estadoSwitch = 12;
      break;
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
    LCD.print("*");
    estadoPrevioSwitch = 8;

    encoder();
    if(digitalRead(pulsEncoder) == HIGH){ // Función para selec. "empezar grabado"
      estadoSwitch = NULL;
      break;
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
    LCD.print("*");

    encoder();
    if(digitalRead(pulsEncoder) == HIGH){ // Función para selec. "pausar grabado"
      estadoSwitch = NULL;
      break;
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
    LCD.print("*");
    estadoPrevioSwitch = 10;

    encoder();
    if(digitalRead(pulsEncoder) == HIGH){ // Función para selec. "terminar grabado"
      estadoSwitch = NULL;
      break;
    }
    
    break;

  case 11:

    if(estadoPrevioSwitch == 10){
      estadoSwitch = 6;
    }

    if(estadoPrevioSwitch == 12){
      estadoSwitch = 12;
    }

    break;
  
  case 12:

    if(a1){
      LCD.clear();
      a1 = 0;
      a2 = 1;
    }
    
    LCD.setCursor(3,1);
    LCD.print("Version 1.0");
    LCD.setCursor(0,3);
    LCD.print("Derechos reservados");
    estadoPrevioSwitch = 12;
    
    if(digitalRead(pulsEncoder) == HIGH){
      estadoSwitch = 1;
    }

    break;
  
  case 13:

    if(a2){
      LCD.clear();
      a1 = 1;
      a2 = 0;
    }

    LCD.setCursor(0,0);
    LCD.print("Creadores:");
    LCD.setCursor(0,1);
    LCD.print("- Barcia");
    LCD.setCursor(0,2);
    LCD.print("- Iglesario");
    LCD.setCursor(0,3);
    LCD.print("- Lentini");
    estadoPrevioSwitch = 13;

    if(digitalRead(pulsEncoder) == HIGH){
      estadoSwitch = 1;
    }

    break;

  case 14:

    if(estadoPrevioSwitch == 13){
      estadoSwitch = 13;
    }

    break;
  }

  // Habra tantos casos como pantallas de opciones haya, estas pantallas se generan por cualquier cambio hecho
  // Los casos tienen que poder repetirse unicamente desde el 1 hasta el numero que se llegue
  // El caso 0 es unicamente el comienzo visual del programa mientras que el caso 1 es el menu principal

}

////--------------------------------------Función Encoder--------------------------------------

void encoder(){

  posicionAhora = digitalRead(clk); 
  
  if (posicionAhora != posicionAntes){     
    if (digitalRead(DT) != posicionAhora) { 
      deteccionCambioSwitch1 = true;
      delay(250);
      if(deteccionCambioSwitch1){
        estadoSwitch++;
      }
      deteccionCambioSwitch1 = false;
    }
    else {
      deteccionCambioSwitch2 = true;
      delay(250);
      if(deteccionCambioSwitch2){
        estadoSwitch--;
      }
      deteccionCambioSwitch2 = false;
    }
  }

}

////--------------------------------------Funciones Lectura e Interpretación--------------------------------------

void lectura_SD(){

  

}

void interpretacion_SD(){

}

//--------------------------------------Funciones PaP--------------------------------------

void movimiento_PaP(){ // Supongamos base de 450mmx450mm
  float diferencia1, diferencia2;
  if(coordenadas[0] != coordenadasPrevias[0]){ // 25 pasos equivalen a 1mm
    diferencia1 = coordenadas[0] - coordenadasPrevias[0];
    myStepper1.step(diferencia1 * 25);
  }
  if(coordenadas[1] != coordenadasPrevias[1]){ // 25 pasos equivalen a 1mm
    diferencia2 = coordenadas[0] - coordenadasPrevias[0];
    myStepper2.step(diferencia2 * 25);
  }
}

void retorno_Al_Home(){

  myStepper1.step(-coordenadas[0] * 25);
  myStepper2.step(-coordenadas[1] * 25);

}

////--------------------------------------Interrupción Externa--------------------------------------

void interrupcion_Externa(){
  
}

////--------------------------------------Timer2--------------------------------------

void timer(){

}
