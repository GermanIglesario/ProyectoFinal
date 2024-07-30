// El programa mas falopa que vas a ver en tu vida, con muy probablemente mas de 2000 lineas de codigo de pura
// mariguana de una calidad inigualable. VLLC! 10/04/2024

// La maquina tiene una presición de 40 micrometros por paso

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

const int pasosPorRevolucion = 200; // Segun los grados
Stepper myStepper1(pasosPorRevolucion, 9, 10, 11, 12);
Stepper myStepper2(pasosPorRevolucion, 5, 6, 7, 8);

File archivo;
File archivos[4];

LiquidCrystal_I2C LCD(0x27, 20, 4); // Pines del LCD 20x4 (pines ?)

////--------------------------------------Declaraciones e inicializaciones de variables--------------------------------------

//--------------------------------------Coordenadas e interpretación--------------------------------------

char cadena[100]; // Concatenación?
char caracter; // Variable receptora
char anteriorCaracter;
int ultimaPosicion = 0; // Guarda la ultima posición del cursor en el archivo
int bytesTotales = 2; // Cantidad de bits que tiene el archivo (el 2 es un valor para engañar al bucle de repetición hasta conseguir el verdadero tamaño del archivo)
float coordenadas[3]; // Del punto a grabar
float coordenadasPrevias[3]; // Del punto anterior
String nombresArc[4];
int coordenadaX1;
int coordenadaY1;
int coordenadaZ1;

//--------------------------------------PaP y relacionados--------------------------------------

float finalX; // Variable de la coordenada X
float finalY; // Variable de la coordenada Y
float finalZ; // Variable de la coordenada Z
int RPM = 60; // Revoluciones por minuto del motor paso a paso
int posC; // Posición para guardar coordenadas

//--------------------------------------Pulsdores y funcionamiento del menu--------------------------------------

int puls1 = 5; // Pulsador de Selección
int puls2 = 6; // Pulsador Decrementor
int puls3 = 7; // Pulsador Incrementor
int estadoSwitch = 0; // Estado del switch segun el encoder
int estadoPrevioSwitch = 0; // Estado previo del switch segun el encoder
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

void movimientoPulsadores();
void deteccion_Archivos();
void lectura_SD();
void interpretacion_SD();
void coordenadaX(int posC);
void coordenadaY(int posC);
void coordenadaZ(int posC);
void movimiento_PaP();
void retorno_Al_Home();
void interrupcion_Externa();
void timer();

////--------------------------------------Setup--------------------------------------

void setup() {

  attachInterrupt(digitalPinToInterrupt(5), interrupcion_Externa, LOW); // Interrupción externa (falta definir PIN)
  MsTimer2::set(1, timer); // Interrupción cada 1ms
  MsTimer2::start(); // Inicio timer
  LCD.init(); // LCD
  LCD.backlight();
  SD.begin(4); // SD (pin al que esta vinculado el modulo SD)
  archivo = SD.open("archivo.txt");
  pinMode(puls1, INPUT); // Pulsadores
  pinMode(puls2, INPUT);
  pinMode(puls3, INPUT);
  myStepper1.setSpeed(50); // Stepper 1
  myStepper2.setSpeed(50); // Stepper 2

}

////--------------------------------------Loop--------------------------------------

void loop() {
  
  //-----------------------------------Pantalla-----------------------------------

  // Caso 0: Introducción
  // Caso 1: Menu Principal
  // Caso 2 al 10: Menu de Opciones
  // Caso 12 al 15: Opcion "Info del Producto"
  // Caso 17 al 18: Selección de Archivo
  // Caso 47 al ???: Extensión del Menu de Opciones
  
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
    LCD.setCursor(3,1);
    LCD.print("Menu Principal");
    LCD.setCursor(5,2);
    LCD.print("en proceso");
    //
    // Pantalla principal (pendiente)
    //
    while(1){ // Permanencia en el menu
      estadoSwitch = 1;
      if(digitalRead(puls1) == HIGH){
        estadoSwitch = 2;
        delay(250);
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

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){
      estadoPrevioSwitch = 0;
      estadoSwitch = 1;
      delay(250);
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

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "empezar grabado"
      estadoSwitch = 17;
      delay(250);
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

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "pausar grabado"
      estadoSwitch = NULL;
    }
    
    break;
  
  case 5:

    if(estadoPrevioSwitch == 6){
      estadoSwitch = 10;
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

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "terminar grabado"
      estadoSwitch = NULL;
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
    LCD.print("- Coord. manuales");

    LCD.setCursor(17,3);
    LCD.print("*");
    estadoPrevioSwitch = 6;

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "coord. manuales"
      estadoSwitch = NULL;
    }
    
    break;

  case 7:

    if(estadoPrevioSwitch == 6){
      estadoSwitch = 50;
    }
    if(estadoPrevioSwitch == 8){
      estadoSwitch = 2;
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
    LCD.print("- Coord. manuales");

    LCD.setCursor(17,0);
    LCD.print("*");
    estadoPrevioSwitch = 8;

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "empezar grabado"
      estadoSwitch = NULL;
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
    LCD.print("- Coord. manuales");

    LCD.setCursor(16,1);
    LCD.print("*");

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "pausar grabado"
      estadoSwitch = NULL;
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
    LCD.print("- Coord. manuales");

    LCD.setCursor(18,2);
    LCD.print("*");
    estadoPrevioSwitch = 10;

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "terminar grabado"
      estadoSwitch = NULL;
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

    if(a2){
      LCD.clear();
      a1 = 1;
      a2 = 0;
    }
    
    LCD.setCursor(3,0);
    LCD.print("Version 1.0");
    LCD.setCursor(0,2);
    LCD.print("Derechos reservados");
    LCD.setCursor(8,3);
    LCD.print("©");
    estadoPrevioSwitch = 12;


    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){
      estadoSwitch = 1;
      delay(250);
    }

    break;
  
  case 13:

    if(a1){
      LCD.clear();
      a1 = 0;
      a2 = 1;
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

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){
      a1 = 1, a2 = 0;
      estadoSwitch = 1;
      delay(250);
    }

    break;

  case 14:

    if(estadoPrevioSwitch == 13){
      estadoSwitch = 13;
    }

    break;

  case 15:

    

    break;
  
  case 16:

    if(estadoPrevioSwitch == 15){
      estadoSwitch = 15;
    }

    if(estadoPrevioSwitch == 17){
      estadoSwitch = 17;
    }

    break;

  case 17:

    if(a2){
      LCD.clear();
      a1 = 1;
      a2 = 0;
    }

    deteccion_Archivos();
    /*LCD.setCursor(1,0);
    LCD.print(nombreArc1);
    */
    estadoPrevioSwitch = 17;

    movimientoPulsadores();

    break;

  case 18:

    // Mostrar archivo seleccionado con nombre completo?
    estadoPrevioSwitch = 18;

    break;
  
  case 19:

    if(estadoPrevioSwitch == 18){
      estadoSwitch = 18;
    }

    break;

  case 46:

    if(estadoPrevioSwitch == 47){
      estadoSwitch = 8;
    }

    break;
  
  case 47:

    if(a2){
      LCD.clear();
      a1 = 1;
      a2 = 0;
    }
    
    LCD.setCursor(0,0);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,1);
    LCD.print("- Terminar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Coord. manuales");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(16,0);
    LCD.print("*");
    estadoPrevioSwitch = 47;

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "pausar grabado"
      estadoSwitch = NULL;
    }

    break;

  case 48:

    if(a1){
      LCD.clear();
      a1 = 0;
      a2 = 1;
    }
    
    LCD.setCursor(0,0);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,1);
    LCD.print("- Terminar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Coord. manuales");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(18,1);
    LCD.print("*");

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "terminar grabado"
      estadoSwitch = NULL;
    }

    break;

  case 49:

    if(a2){
      LCD.clear();
      a1 = 1;
      a2 = 0;
    }
    
    LCD.setCursor(0,0);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,1);
    LCD.print("- Terminar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Coord. manuales");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(17,2);
    LCD.print("*");

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "coord. manuales"
      estadoSwitch = NULL;
    }

    break;

  case 50:

    if(a1){
      LCD.clear();
      a1 = 0;
      a2 = 1;
    }
    
    LCD.setCursor(0,0);
    LCD.print("- Pausar grabado");
    LCD.setCursor(0,1);
    LCD.print("- Terminar grabado");
    LCD.setCursor(0,2);
    LCD.print("- Coord. manuales");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(19,3);
    LCD.print("*");
    estadoPrevioSwitch = 50;

    movimientoPulsadores();
    if(digitalRead(puls1) == HIGH){ // Función para selec. "info del producto"
      estadoSwitch = 12;
      delay(250);
    }

    break;

  case 51:

    if(estadoPrevioSwitch == 50){
      estadoSwitch = 50;
    }

    break;

  }
  
  // Habra tantos casos como pantallas de opciones haya, estas pantallas se generan por cualquier cambio hecho
  // Los casos tienen que poder repetirse unicamente desde el 1 hasta el numero que se llegue
  // El caso 0 es unicamente el comienzo visual del programa mientras que el caso 1 es el menu principal

}

////--------------------------------------Función Pulsadores--------------------------------------

void movimientoPulsadores(){

  if(digitalRead(puls2) == HIGH){
    estadoSwitch++;
    delay(200);
  }
  if(digitalRead(puls3) == HIGH){
    estadoSwitch--;
    delay(200);
  }

}

////--------------------------------------Funciones Lectura e Interpretación--------------------------------------

void deteccion_Archivos(){

  if(archivo){
    for(int i = 0; i < 4; i++){
      nombresArc[i] = archivo.name();
      archivo.close();
      archivo = archivo.openNextFile();
    }
  }

  LCD.setCursor(0,0);
  LCD.print(nombresArc[0]);
  LCD.setCursor(0,1);
  LCD.print(nombresArc[1]);
  LCD.setCursor(0,2);
  LCD.print(nombresArc[2]);
  LCD.setCursor(0,3);
  LCD.print(nombresArc[3]);

}

void lectura_SD(String nombre){ // La función recibe como parametro el nombre del archivo a leer

  for(int i = 0; i < 100; i++){ // Bucle para eliminar la cadena leida en el ciclo anterior
    cadena[i] = NULL;
  }
  
  archivo = SD.open("archivo.txt"); // Se abre el archivo
  bytesTotales = archivo.size(); // Se guarda el tamaño del mismo
  
  if(archivo){ // Si existe
    if(ultimaPosicion <= bytesTotales){ // Si no se termino de recorrer
      if(archivo.available()){ // Si esta disponible
        for(int i = 0 ; archivo.read() != 10; i++, ultimaPosicion++){ // El 10 es el enter en tabla ASCII
          archivo.seek(ultimaPosicion); // Se ubica en el archivo
          cadena[i] = archivo.read(); // Lo lee y guarda
        }
        ultimaPosicion++; // Salta a la linea de abajo
      }
    }
  }

  archivo.close();
  interpretacion_SD();
  
}

void interpretacion_SD(){ // La función que interpreta el archivo gcode posterior a obtener una de sus lineas

  for(int i = 0; cadena[i] != 10; i++){ // Un for que se repite hasta que se termine la linea (detectando el enter)
  int aux1, aux2;
    if(cadena[i] >= '0' && cadena[i] <= '9' && anteriorCaracter == 'X'){ // Un if para guardar X
      coordenadaX(i);
    }
    if(cadena[i] >= '0' && cadena[i] <= '9' && anteriorCaracter == 'Y'){ // Un if para guardar Y
      coordenadaY(i);
    }
    if(cadena[i] >= '0' && cadena[i] <= '9' && anteriorCaracter == 'Z'){ // Un if para guardar Z
      coordenadaZ(i);
    }
    anteriorCaracter = cadena[i - 1];
  }

}

void coordenadaX(int posC){ // Función para almacenar la coordenada X

  // Guardado de la parte entera
  int aux1, aux2, aux3, aux4;
  for(; 1; posC++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;  
    if(cadena[posC] == '.'){
      break;
    }
    aux4 = cadena[posC] - '0';
  }
  
  finalX = aux1 * 100 + aux2 * 10 + aux3 * 1;

  // Guardado de la parte decimal
  aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0;
  int posC2 = posC + 1, i = 0;
  for(; 1; posC2++, i++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;  
    if(cadena[posC2] == ' '){
      break;
    }
    aux4 = cadena[posC2] - '0';
  }

  if(i == 1){
    finalX += aux3 * 0.1;
  }
  if(i == 2){
    finalX += aux2 * 0.1 + aux3 * 0.01;
  }
  if(i == 3){
    finalX += aux1 * 0.1 + aux2 * 0.01 + aux3 * 0.001;
  }

}

void coordenadaY(int posC){ // Función para almacenar la coordenada Y

  // Guardado de la parte entera
  int aux1, aux2, aux3, aux4;
  for(; 1; posC++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;  
    if(cadena[posC] == '.'){
      break;
    }
    aux4 = cadena[posC] - '0';
  }
  
  finalY = aux1 * 100 + aux2 * 10 + aux3 * 1;

  // Guardado de la parte decimal
  aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0;
  int posC2 = posC + 1, i = 0;
  for(; 1; posC2++, i++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;  
    if(cadena[posC2] == ' '){
      break;
    }
    aux4 = cadena[posC2] - '0';
  }

  if(i == 1){
    finalY += aux3 * 0.1;
  }
  if(i == 2){
    finalY += aux2 * 0.1 + aux3 * 0.01;
  }
  if(i == 3){
    finalY += aux1 * 0.1 + aux2 * 0.01 + aux3 * 0.001;
  }

}

void coordenadaZ(int posC){ // Función para almacenar la coordenada Z

// Guardado de la parte entera
  int aux1, aux2, aux3, aux4;
  for(; 1; posC++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;  
    if(cadena[posC] == '.'){
      break;
    }
    aux4 = cadena[posC] - '0';
  }
  
  finalZ = aux1 * 100 + aux2 * 10 + aux3 * 1;

  // Guardado de la parte decimal
  aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0;
  int posC2 = posC + 1, i = 0;
  for(; 1; posC2++, i++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;  
    if(cadena[posC2] == ' '){
      break;
    }
    aux4 = cadena[posC2] - '0';
  }

  if(i == 1){
    finalZ += aux3 * 0.1;
  }
  if(i == 2){
    finalZ += aux2 * 0.1 + aux3 * 0.01;
  }
  if(i == 3){
    finalZ += aux1 * 0.1 + aux2 * 0.01 + aux3 * 0.001;
  }

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
