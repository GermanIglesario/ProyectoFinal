// La maquina tiene una presición de 40 micrometros por paso

#include <Ticker.h>
#include <ESP32Servo.h>
#include <string.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>

////-------------------------------------------------------Servo + Pantalla---------------------------------------------------------------------------

LiquidCrystal_I2C LCD(0x27, 20, 4); // Pines del LCD 20x4 (pines ?)
Servo servo;

////--------------------------------------Declaraciones e inicializaciones de variables--------------------------------------

//---------------------------------------Variables orientadas a la función de origen-----------------------------------------

#define direccionX 14
#define direccionY 26
#define stepsX 12
#define stepsY 27
#define endStpX 32
#define endStpY 33
#define POT 15
#define LED 16

////----------------------------------------------------INTERRUPCION POR TIMER------------------------------------------

Ticker tiempo_Pulsadores;
bool puls_Time = LOW;

////----------------------------------------------VARIABLES ORIENTADAS A LA NIVELACION----------------------------------

bool ESStateX = 0;
bool ESStateY = 0;
bool FEDX = 0;
bool FEDY = 0;

int valorPote = 0;
int ang;

int state = 1;

bool flag1 = 0;
bool flag2 = 0;

bool flagOrigin = 0;
bool flagOrigin2 = 0;

bool level_Position = 0;

int j = 1;
int a = 0;

bool printFlag = 0;

int control_Nivel = 0;

bool variable_Cambio = 0;

//--------------------------------------Coordenadas e interpretación--------------------------------------

File archivo;
File archivos[4];
char cadena[100];
char anteriorCaracter;
float finalX = 0, finalY = 0, finalZ = 0, finalAnteriorX = 0, finalAnteriorY = 0;
float diferenciaX;
float diferenciaY;
int ultimaPosicion = 0; // Guarda la ultima posición del cursor en el archivo
int bytesTotales = 2;
int aGrabado = 1;
int cantidadRepeticiones = 0;
bool sentidoX = 0;
bool sentidoY = 0;
int delayMotores;
#define delayGrabado 200
int repeticiones_Nivel = 769;



// Pin del chip select del lector de tarjetas SD
#define chipSelect 5

//--------------------------------------Pulsdores y funcionamiento del menu--------------------------------------

#define puls1 4 // Pulsador de Selección
#define puls2 17 // Pulsador Decrementor
#define puls3 2 // Pulsador Incrementor
int estadoSwitch = 0; // Estado del switch segun el encoder
int estadoPrevioSwitch = 0; // Estado previo del switch segun el encoder
bool deteccionCambioSwitch1; // Variable boleana que le dice que valor tomar a estadoSwitch
bool deteccionCambioSwitch2; // Variable boleana que le dice que valor tomar a estadoSwitch
bool flagTiempo = 0; // Variable para hacer que el pulsador 1 no tenga retención
int e = 1; // Variable de unica vez, pantalla de inicio
int a1 = 1; // Variable de unica vez, limpiar la pantalla
int a2 = 1; // Variable de unica vez, limpiar la pantalla

////--------------------------------------Prototipado Funciones--------------------------------------

void movimientoPulsadores();
void deteccion_Archivos();
void lectura_SD();
void interpretacion_SD();
void coordenadaX(int posC);
void coordenadaY(int posC);
void coordenadaZ(int posC);
void movimiento_PaP();
void interrupcion_Externa();
void inicio_De_Nivelacion_Origen();
void origen();
void punto_Nivel();
void regulacion_Eje_Z();
void parpadeo_Leds();
void reestablecimiento_Variables();


////--------------------------------------Setup--------------------------------------

void setup() {

  LCD.init(); // LCD
  LCD.backlight();

  SD.begin(5); // SD (pin al que esta vinculado el modulo SD)
  archivo = SD.open("/archivo.txt", FILE_READ); // Se abre el archivo
  
  pinMode(puls1, INPUT_PULLDOWN); // Pulsadores
  pinMode(puls2, INPUT_PULLDOWN);
  pinMode(puls3, INPUT_PULLDOWN);
  pinMode(POT, INPUT);
  pinMode(LED, OUTPUT);

  servo.attach(25); //Inicializa el servo en el pin 25
  Serial.begin(9600);

  pinMode(direccionX, OUTPUT);
  pinMode(stepsX, OUTPUT);
  pinMode(direccionY, OUTPUT);
  pinMode(stepsY, OUTPUT);

  pinMode(endStpX, INPUT_PULLDOWN);
  pinMode(endStpY, INPUT_PULLDOWN);

  tiempo_Pulsadores.attach(0.12, timer_Puls);

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

    digitalWrite(LED, HIGH);

    if(e){
      LCD.setCursor(0,1);
      LCD.print("Grabadora Laser CNC");
      LCD.setCursor(1,3);
      LCD.print("Fefi's production");

      delay(1000);

      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("Creada por:");
      LCD.setCursor(0,1);
      LCD.print("- Barcia");
      LCD.setCursor(0,2);
      LCD.print("- Iglesario");
      LCD.setCursor(0,3);
      LCD.print("- Lentini");

      delay(1000);
    
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
    LCD.setCursor(5,1);
    LCD.print("Bienvenido");
    LCD.setCursor(3,2);
    LCD.print("Presione ENTER");

    while(1){ // Permanencia en el menu
      estadoSwitch = 1;
      if(digitalRead(puls1) && puls_Time && !flagTiempo){
        estadoSwitch = 2;
        flagTiempo = 1;
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
    LCD.print("- Nivelacion");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");



    LCD.setCursor(16,0);
    LCD.print("*");
    estadoPrevioSwitch = 2;

    movimientoPulsadores();
    if(digitalRead(puls1) && puls_Time && !flagTiempo){

      estadoPrevioSwitch = 0;
      estadoSwitch = 1;
      flagTiempo = 1;
    
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
    LCD.print("- Nivelacion");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(17,1);
    LCD.print("*");

    movimientoPulsadores();
    if(digitalRead(puls1) && puls_Time && !flagTiempo){ // Función para selec. "empezar grabado"
      
      estadoSwitch = 17;
      flagTiempo = 1;

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
    LCD.print("- Nivelacion");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(12,2);
    LCD.print("*");

    movimientoPulsadores();
    if(digitalRead(puls1) && puls_Time && !flagTiempo){ // Función para selec. "nivelación"
      estadoSwitch = 100;
      flagTiempo = 1;
    }
    
    break;
  
  case 5:

   

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
    LCD.print("- Nivelacion");
    LCD.setCursor(0,3);
    LCD.print("- Info del producto");

    LCD.setCursor(19,3);
    LCD.print("*");
    estadoPrevioSwitch = 5;

    movimientoPulsadores();
    if(digitalRead(puls1) && puls_Time && !flagTiempo){ // Función para selec. "info del producto"
      estadoSwitch = 12;
      flagTiempo = 1;
    }
    
    break;
  
  case 6:

    if(estadoPrevioSwitch == 5){ // Extensión del Menu bloqueada
      estadoSwitch = 5;
      break;
    }
    
    break;

  case 11:

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
    if(digitalRead(puls1) && puls_Time && !flagTiempo){
      
      estadoSwitch = 1;
      flagTiempo = 1;
      
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
    if(digitalRead(puls1) && puls_Time && !flagTiempo){
      
      a1 = 1, a2 = 0;
      estadoSwitch = 1;
      flagTiempo = 1;
      
    }

    break;

  case 14:

    if(estadoPrevioSwitch == 13){
      estadoSwitch = 13;
    }

    break;
  
  case 16:

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

    Serial.println("");
    Serial.print("Print Flag: ");
    Serial.print(printFlag);
    Serial.println("");

    if(!printFlag){

      LCD.clear();
      LCD.setCursor(1,1);
      LCD.print("EQUIPO SIN NIVELAR");
      delay(2000);

      estadoSwitch = 2;

      if(a1){
        LCD.clear();
        a1 = 0;
        a2 = 1;
      }

    }

    if(printFlag){

      variable_Cambio = 0;

      LCD.setCursor(0,1);
      LCD.print("-Pausar grabado");
      LCD.setCursor(0,2);
      LCD.print("-Terminar grabado");

      
      if(aGrabado){
        lectura_SD();
      }
      if(!aGrabado){
        reestablecimiento_Variables();
      }  

    }

    if(variable_Cambio){
      
      estadoSwitch = 2;
      aGrabado = 1;

    }

    estadoPrevioSwitch = 17;

    movimientoPulsadores();

    break;

  case 100:

    Serial.println("");
    Serial.println("Estoy en case 100");

    if(!printFlag){

      inicio_De_Nivelacion_Origen();

    }

    if(printFlag){

      delayMotores = 100;

      estadoSwitch = 2;

    }
    
    break;

  }
  
  // Habra tantos casos como pantallas de opciones haya, estas pantallas se generan por cualquier cambio hecho
  // Los casos tienen que poder repetirse unicamente desde el 1 hasta el numero que se llegue
  // El caso 0 es unicamente el comienzo visual del programa mientras que el caso 1 es el menu principal

}

////--------------------------------------Función Pulsadores--------------------------------------

void movimientoPulsadores(){

  if(digitalRead(puls2) && puls_Time){
    
    estadoSwitch++;

  }
  if(digitalRead(puls3) && puls_Time){
    
    estadoSwitch--;

  }

}

////--------------------------------------------------Funciones de la interrupción por timer-----------------------------------------------------------------

void timer_Puls(){

  puls_Time = !puls_Time;

  if(estadoSwitch == 17){

    digitalWrite(LED, puls_Time);

  }

  if(flagTiempo){
    if(!digitalRead(puls1)){
      flagTiempo = 0;
    }
  }

}

//----------------------------------------------Funciones orientadas a la nivelación del dispositivo---------------------------------------------------------

void inicio_De_Nivelacion_Origen(){

  delayMotores = 40;


  switch(control_Nivel){

    case 0:

      if(!flagOrigin){

        servo.write(0);
        delay(500);
        origen();

      }
      
      if(flagOrigin){

        control_Nivel = 2;

      }

      Serial.println(control_Nivel);

    break;

    case 2:

      Serial.println("ESTOY");

      LCD.clear();
      LCD.setCursor(0,0);
      LCD.print("EN ORIGEN");
      delay(1500);

      punto_Nivel();

      j = 1;
      a = 0;
      flagOrigin2 = 1;
      regulacion_Eje_Z();

    break;

    case 3:

      if(a){

          ESStateX = 0;
          ESStateY = 0;

          FEDX = 0;
          FEDY = 0;

          if(flagOrigin2){
            
            origen();

          }

          if(!flagOrigin2){

            LCD.clear();
            LCD.setCursor(0,0);
            LCD.print("DONE!");
            delay(1000);

            printFlag = 1;

          }

      }

    break;

  }


}

//---------------------------------------------------------------------funcion de origen----------------------------------------------------------------------------

void origen(){

  LCD.clear();
  LCD.setCursor(0, 0);
  LCD.print("Viaje a Origen...");

  while(!FEDX){

    if(digitalRead(endStpX) == HIGH){

      ESStateX = 1;
      FEDX = 1;
          
    }
    
    digitalWrite(direccionX, LOW);    
     

    digitalWrite(stepsX, HIGH);
    delayMicroseconds(delayMotores);
    digitalWrite(stepsX, LOW);
    delayMicroseconds(delayMotores);
  
  }

  while(!FEDY){

    if(digitalRead(endStpY) == HIGH){

      ESStateY = 1; 
      FEDY = 1;
   
    }

    digitalWrite(direccionY, LOW);    

    digitalWrite(stepsY, HIGH);
    delayMicroseconds(delayMotores);
    digitalWrite(stepsY, LOW);
    delayMicroseconds(delayMotores);

  }

  if(ESStateY == 1 && ESStateX == 1){

    flagOrigin = 1;
    flagOrigin2 = 0;

  }
}

//--------------------------------------------------------------funcion punto nivel-------------------------------------------------------------------------------

void punto_Nivel(){

  LCD.clear();
  LCD.setCursor(0,0);
  LCD.print("Desplazando...");

  digitalWrite(direccionY, HIGH);


  for(int i = 0; i < repeticiones_Nivel; i++){

    for(int j = 0; j < 200; j++){

      digitalWrite(stepsY, HIGH);         
      delayMicroseconds(delayMotores);
      digitalWrite(stepsY, LOW); 
      delayMicroseconds(delayMotores);

    }
  }

  delay(1500);

  level_Position = 1;


}

//----------------------------------------------------------------funcion regulacion de eje Z--------------------------------------------------------------------

void regulacion_Eje_Z(){

  if(!a){

    if(digitalRead(puls1) && puls_Time){
  
      state = !state;
      
    }
    
  
    if(state){
      
      servo.write(0); 
  
      if(digitalRead(puls1) && puls_Time){
    
        state = !state;
        
      }
  
      valorPote = analogRead(POT);
  
      ang = map(valorPote, 0, 4095, 0, 180);

      if(ang >= 0 && ang <= 25){
  
        j = 0;
          
      }
  
      while(valorPote >= 568 && valorPote <= 4095 && j == 1){
  
  
        valorPote = analogRead(POT);
  
        ang = map(valorPote, 0, 4095, 0, 180);
  
        LCD.clear();
        LCD.setCursor(0,0);
        LCD.print("SET REG TO MIN");
        delay(150);
  
        if(valorPote >= 0 && valorPote <= 568){
  
          j = 0;
          
        }
      }

      if(!j){

        LCD.clear();
        LCD.setCursor(0,0);
        LCD.print("PRESS OK TO SET");

      }
  
      while(j == 0 && a == 0){
        
        valorPote = analogRead(POT);
  
        ang = map(valorPote, 0, 4095, 0, 180);
  
        servo.write(ang);  

        Serial.println(ang);

        if(digitalRead(puls1) == HIGH){
          servo.write(0);
          a = 1;
        }
      }
    }
  
    if(!state){

      if(digitalRead(puls1) && puls_Time){

        state = !state;
        j = 1;

      }
      LCD.setCursor(0,0);
      LCD.print("PRESS BUTTON");
      delay(200);
    }
  }

  if(a){

    control_Nivel = 3;

    LCD.clear();
    LCD.setCursor(0,0);
    LCD.print("FINISHED");
    delay(1500);
  }

}

////--------------------------------------------Funciones orientadas a la interpretacion y ejecucion del grabado-----------------------------------------------

void lectura_SD(){ // La función recibe como parametro el nombre del archivo a leer

  for(int i = 0; i < 100; i++){ // Bucle para eliminar la cadena leida en el ciclo anterior
    cadena[i] = ' ';
  }

  // Las coordenadas actuales pasan a ser las previas para que se pueda calcular la diferencia entre las mismas
  finalAnteriorX = finalX;
  finalAnteriorY = finalY;
  
  
  archivo = SD.open("/archivo.txt", FILE_READ); // Se abre el archivo
  bytesTotales = archivo.size(); // Se guarda el tamaño del mismo
  //Serial.println(bytesTotales);

  if(archivo){ // Si existe
    if(ultimaPosicion <= bytesTotales){ // Si no se termino de recorrer
      if(archivo.available()){ // Si esta disponible
        for(int i = 0 ; cadena[i - 1] != 10; i++, ultimaPosicion++){ // El 10 es el enter en tabla ASCII
          archivo.seek(ultimaPosicion); // Se ubica en el archivo
          cadena[i] = archivo.read(); // Lo lee y guarda
          if(ultimaPosicion == bytesTotales){ // Si se llega al final del archivo se sale del for antes de que se trabe
            break;
          }
          //Serial.println(cadena[i]);
        }
        ultimaPosicion++; // Salta a la linea de abajo
        if(ultimaPosicion == bytesTotales + 1){ // Al llegar al final del archivo se resetea la variable ultima posición y se cambia una flag para que se deje de iterar la secuencia
          ultimaPosicion = 0;
          aGrabado = 0;
        }
      }
    }
  }
  //Serial.println("Salio señor");
  archivo.close();
  interpretacion_SD();
  
}

//------------------------------------------------------------Funcion interpretacion SD-------------------------------------------------------------------------------

void interpretacion_SD(){ // La función que interpreta el archivo gcode posterior a obtener una de sus lineas

  for(int i = 0; cadena[i] != 10; i++){ // Un for que se repite hasta que se termine la linea (detectando el enter)
    //Serial.println(cadena[i]);
    if(i >= 1){
      anteriorCaracter = cadena[i - 1];
    }
    if(cadena[i] >= '0' && cadena[i] <= '9' && anteriorCaracter == 'X'){ // Un if para guardar X
      //Serial.println("Entro señor");
      coordenadaX(i);
    }
    if(cadena[i] >= '0' && cadena[i] <= '9' && anteriorCaracter == 'Y'){ // Un if para guardar Y
      //Serial.println("Entro señor");
      coordenadaY(i);
    }
    if(cadena[i] >= '0' && cadena[i] <= '9' && anteriorCaracter == 'Z'){ // Un if para guardar Z
      //Serial.println("Entro señor");
      coordenadaZ(i, 0);
    }
    if(cadena[i + 1] >= '0' && cadena[i + 1] <= '9' && cadena[i] == '-' && anteriorCaracter == 'Z'){
      //Serial.println("Entro señor");
      coordenadaZ(i + 1, 1);
    }
  }

  movimiento_Motores();

}

//---------------------------------------------------------------Funcion cordenadaX------------------------------------------------------------------------------

void coordenadaX(int posC){ // Función para almacenar la coordenada X

  // Guardado de la parte entera
  int aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0;

  for(; 1; posC++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;  
    if(cadena[posC] == '.'){
      break;
    }
    aux4 = cadena[posC] - '0';
    //Serial.println(aux4);
  }
  
  finalX = aux1 * 100 + aux2 * 10 + aux3 * 1;

  //Serial.print(finalX);
  
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
    //Serial.println(aux6);
  }

  finalX += aux1 * 0.1 + aux2 * 0.01 + aux3 * 0.001;

  /*if(i == 1){
    finalX += aux3 * 0.1;
  }
  if(i == 2){
    finalX += aux2 * 0.1 + aux3 * 0.01;
  }
  if(i == 3){
    finalX += aux1 * 0.1 + aux2 * 0.01 + aux3 * 0.001;
  }*/

  Serial.println(finalX);

}

//-------------------------------------------------------------------funcion cordenadaY-------------------------------------------------------------------------

void coordenadaY(int posC){ // Función para almacenar la coordenada Y

  // Guardado de la parte entera
  int aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0, aux5 = 0, aux6 = 0;

  for(; 1; posC++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;  
    if(cadena[posC] == '.'){
      break;
    }
    aux4 = cadena[posC] - '0';
    //Serial.println(aux4);
  }
  
  finalY = aux1 * 100 + aux2 * 10 + aux3 * 1;

  //Serial.print(finalY);
  
  // Guardado de la parte decimal
  aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0, aux5 = 0, aux6 = 0;
  int posC2 = posC + 1, i = 0;
  for(; 1; posC2++, i++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;
    if(cadena[posC2] == ' '){
      break;
    }
    aux4 = cadena[posC2] - '0';
    //Serial.println(aux6);
  }

  finalY += aux1 * 0.1 + aux2 * 0.01 + aux3 * 0.001;

  /*if(i == 1){
    finalY += aux3 * 0.1;
  }
  if(i == 2){
    finalY += aux2 * 0.1 + aux3 * 0.01;
  }
  if(i == 3){
    finalY += aux1 * 0.1 + aux2 * 0.01 + aux3 * 0.001;
  }*/

  Serial.println(finalY);

}

//-------------------------------------------------------------------funcion cordenadaZ-------------------------------------------------------------------------


void coordenadaZ(int posC, bool signoMenos){ // Función para almacenar la coordenada Z

  // Guardado de la parte entera
  int aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0, aux5 = 0, aux6 = 0;

  for(; 1; posC++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;  
    if(cadena[posC] == '.'){
      break;
    }
    aux4 = cadena[posC] - '0';
    //Serial.println(aux4);
  }
  
  finalZ = aux1 * 100 + aux2 * 10 + aux3 * 1;

  //Serial.print(finalZ);
  
  // Guardado de la parte decimal
  aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0, aux5 = 0, aux6 = 0;
  int posC2 = posC + 1, i = 0;
  for(; 1; posC2++, i++){
    aux1 = aux2;
    aux2 = aux3;
    aux3 = aux4;
    aux4 = aux5;
    aux5 = aux6;
    if(cadena[posC2] == ' '){
      break;
    }
    aux6 = cadena[posC2] - '0';
    //Serial.println(aux6);
  }

  finalZ += aux1 * 0.1 + aux2 * 0.01 + aux3 * 0.001;

  if(signoMenos){
    finalZ *= -1;
  }

  /*if(i == 1){
    finalZ += aux3 * 0.1;
  }
  if(i == 2){
    finalZ += aux2 * 0.1 + aux3 * 0.01;
  }
  if(i == 3){
    finalZ += aux1 * 0.1 + aux2 * 0.01 + aux3 * 0.001;
  }*/

  Serial.println(finalZ);

}

//-------------------------------------------------------------------funcion movimiento de Motores------------------------------------------------------------------------


void movimiento_Motores(){

  // Se obtiene la diferencia entre la coordenada de la linea actual y la de la linea anterior
  diferenciaX = finalX - finalAnteriorX;
  diferenciaY = finalY - finalAnteriorY;

  // Se define el sentido de los motores dependiendo de si la diferencia entre la coordenada actual y anterior es positiva o negativa
  if(diferenciaX >= 0){

    sentidoX = HIGH;

  }
  if(diferenciaY >= 0){

    sentidoY = HIGH;

  }
  if(diferenciaX <= 0){

    sentidoX = LOW;

  }
  if(diferenciaY <= 0){

    sentidoY = LOW;

  }

  digitalWrite(direccionX, sentidoX);
  digitalWrite(direccionY, sentidoY);


  //Serial.println("X: ");
  //Serial.print(direccionX);

  //Serial.println("Y: ");
  //Serial.print(direccionY);

  // Volvemos ambas diferencias positivas debido al uso que se le dara (cantidad de repeticiones en un bucle)
  if(diferenciaX < 0){
    diferenciaX *= -1;
  }


  if(diferenciaY < 0){
    diferenciaY *= -1;
  }

  Serial.print("DirerenciaX: ");
  Serial.println(diferenciaX);
  Serial.print("DirerenciaY: ");
  Serial.println(diferenciaY);

  cantidadRepeticiones = (diferenciaX * 200 * 16) / 8;

  Serial.print("Cantidad de repeticiones X: ");
  Serial.println(cantidadRepeticiones);

  // Dependiendo de si la Z es positiva eleva el cabezal durante una sola linea
  if(finalZ > 0){
    servo.write(0);
  }
  if(finalZ < 0){
    servo.write(ang);
  }

  // Menu de pausar y terminar grabado externo a los bucles

  if(digitalRead(puls3) == HIGH){
    LCD.clear();
    LCD.setCursor(2,1);
    LCD.print("Grabado pausado*");
    delay(500);
    while(1){
      if(digitalRead(puls3) == HIGH){
        LCD.clear();
        LCD.setCursor(0,1);
        LCD.print("-Pausar grabado");
        LCD.setCursor(0,2);
        LCD.print("-Terminar grabado");
        delay(1000);
        break;
      }
    }
  }

  if(digitalRead(puls2) == HIGH){
    LCD.clear();
    LCD.setCursor(4,1);
    LCD.print("Esta seguro?");
    delay(1500);
    LCD.setCursor(0,2);
    LCD.print("Si(p2)");
    LCD.setCursor(10,2);
    LCD.print("No(p3)");
    while(1){
      if(digitalRead(puls3) == HIGH){
        delay(250);
        aGrabado = 0;
        ultimaPosicion = 0;
        finalX = 0, finalY = 0;
        finalAnteriorX = 0, finalAnteriorY = 0;
        diferenciaX = 0, diferenciaY = 0;
        anteriorCaracter = NULL;

        reestablecimiento_Variables();

        break;
      }
      if(digitalRead(puls2) == HIGH){
        LCD.clear();
        LCD.setCursor(0,1);
        LCD.print("-Pausar grabado");
        LCD.setCursor(0,2);
        LCD.print("-Terminar grabado");
        delay(250);
        break;
      }
    }
  }


  // Bucle que por cada repetición hace avanzar un paso al motor (3200 micro pasos (1/16) = 8~9mm)
  for(int i = 0; i < cantidadRepeticiones; i++){ 
    digitalWrite(stepsX, HIGH);         
    delayMicroseconds(delayGrabado);
    digitalWrite(stepsX, LOW); 
    delayMicroseconds(delayGrabado);
    

    if(digitalRead(puls3) == HIGH){
      LCD.clear();
      LCD.setCursor(2,1);
      LCD.print("Grabado pausado*");
      delay(1000);
      while(1){
        if(digitalRead(puls3) == HIGH){
          LCD.clear();
          LCD.setCursor(0,1);
          LCD.print("-Pausar grabado");
          LCD.setCursor(0,2);
          LCD.print("-Terminar grabado");
          delay(500);
          break;
        }
      }
    }

    if(digitalRead(puls2) == HIGH){
      LCD.clear();
      LCD.setCursor(4,1);
      LCD.print("Esta seguro?");
      delay(1000);
      LCD.setCursor(0,2);
      LCD.print("Si(p2)");
      LCD.setCursor(10,2);
      LCD.print("No(p3)");
      while(1){
        if(digitalRead(puls3) == HIGH){
          delay(250);
          aGrabado = 0;
          ultimaPosicion = 0;
          finalX = 0, finalY = 0;
          finalAnteriorX = 0, finalAnteriorY = 0;
          diferenciaX = 0, diferenciaY = 0;
          anteriorCaracter = NULL;

          reestablecimiento_Variables();

          break;
        }
        if(digitalRead(puls2) == HIGH){
          LCD.clear();
          LCD.setCursor(0,1);
          LCD.print("-Pausar grabado");
          LCD.setCursor(0,2);
          LCD.print("-Terminar grabado");
          delay(250);
          break;
        }
      }
    }
  }

  Serial.print("Cantidad de repeticiones Y: ");
  cantidadRepeticiones = (diferenciaY * 200 * 16) / 8;

  Serial.println(cantidadRepeticiones);

  // Bucle que por cada repetición hace avanzar un paso al motor (3200 micro pasos (1/16) = 8~9mm)
  for(int i = 0; i < cantidadRepeticiones; i++){ 
    digitalWrite(stepsY, HIGH);         
    delayMicroseconds(delayGrabado);
    digitalWrite(stepsY, LOW); 
    delayMicroseconds(delayGrabado);


    if(digitalRead(puls3) == HIGH){
      LCD.clear();
      LCD.setCursor(2,1);
      LCD.print("Grabado pausado*");
      delay(500);
      while(1){
        if(digitalRead(puls3) == HIGH){
          LCD.clear();
          LCD.setCursor(0,1);
          LCD.print("-Pausar grabado");
          LCD.setCursor(0,2);
          LCD.print("-Terminar grabado");
          delay(1000);
          break;
        }
      }
    }

    if(digitalRead(puls2) == HIGH){
      LCD.clear();
      LCD.setCursor(4,1);
      LCD.print("Esta seguro?");
      delay(1000);
      LCD.setCursor(0,2);
      LCD.print("Si(p2)");
      LCD.setCursor(10,2);
      LCD.print("No(p3)");
      while(1){
        if(digitalRead(puls3) == HIGH){
          delay(250);
          aGrabado = 0;
          ultimaPosicion = 0;
          finalX = 0, finalY = 0;
          finalAnteriorX = 0, finalAnteriorY = 0;
          diferenciaX = 0, diferenciaY = 0;
          anteriorCaracter = NULL;

          reestablecimiento_Variables();

          break;
        }
        if(digitalRead(puls2) == HIGH){
          LCD.clear();
          LCD.setCursor(0,1);
          LCD.print("-Pausar grabado");
          LCD.setCursor(0,2);
          LCD.print("-Terminar grabado");
          delay(250);
          break;
        }
      }
    }
  }

}

void reestablecimiento_Variables(){

  finalX = 0; 
  finalY = 0; 
  finalZ = 0; 
  finalAnteriorX = 0; 
  finalAnteriorY = 0;
  sentidoX = 0;
  sentidoY = 0;
  ultimaPosicion = 0;
  bytesTotales = 2;
  flagOrigin = 0;
  flagOrigin2 = 0;
  ESStateX = 0;
  ESStateY = 0;
  FEDX = 0;
  FEDY = 0;
  a = 0;
  j = 1;
  flagOrigin = 0;
  flagOrigin2 = 0;
  ESStateX = 0;
  ESStateY = 0;
  FEDX = 0;
  FEDY = 0;
  a = 0;
  j = 1;
  control_Nivel = 0;
  printFlag = 0;
  variable_Cambio = 1;
  
}
