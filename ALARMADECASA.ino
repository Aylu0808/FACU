#include <LiquidCrystal_I2C.h> //libreria para el lcd
#include <Keypad.h> //libreria para el teclado
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);// declaro el lcd

int led1 = 5;
int led2 = 6;
int buzzer = 7;
int infra = A0;

const byte FILAS = 4;
const byte COLUMNAS = 4;

char botones[FILAS][COLUMNAS] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
 };

byte PinF[FILAS] = {13,12,11,10}; //pines de las filas que van al arduino
byte PinC[COLUMNAS] = {9,8,7,6}; //pines de las columnas que van al arduino

Keypad keypad = Keypad(makeKeymap(botones), PinF, PinC, FILAS, COLUMNAS); // declaro todo junto
 
int modo= 0; // la variable para el menu de la alarma
int modolcd = 0;
String entrada; //almacena los datos
String Contra = "6528";//contraseña
bool detectado = false; // para ver si entro alguien

//Definir funciones
/*void armar();
void desarmar();
void cambiarContra();
void repetirContra();*/
void mensaje();
void activacion();
void escribir(String text, int col, int fila);

void setup() {
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(infra, INPUT);
  pinMode(buzzer, OUTPUT);

  lcd.init(); //inicializacion del lcd
  lcd.backlight();
  lcd.print("DESACTIVADO");

  Serial.begin(9600);

}

void loop() {

  char key = keypad.getKey(); //Lee la tecla presionada 

  switch(modo) {

    case 0: //Armar

    if (key) {
      entrada += key; // Guarda la tecla en entrada

      if (key != 'A' and key != 'B' and key != 'C' and key != 'D') { // SON LAS TECLAS CON LOS MODOS
        lcd.print('*'); 
      }

      if (key == 'B') { //Si se presiono B se borra la entrada
        lcd.clear();
        entrada = "";
      }

      if (key == 'A') {

        entrada = entrada.substring(0, entrada.length() - 1);

        if (entrada == Contra) { //Si lo que se introdujo es igual a la contraseña inicia la activacion
          activacion();
          digitalWrite(led1, HIGH);
          digitalWrite(led2, HIGH);
          escribir("ACTIVADO", 0, 0);//escribir("ACTIVADO", 0, 0);
          modo = 1; //pasa al siguiente caso que seia activacion
          key = '\0';
          entrada = "";
        } else {
          escribir("Error!", 5, 0); //Si la contraseña no es correcta salta Error
          entrada = "";
          tone(5, 100, 1000); //Suena el buzzer
          delay(1000);
          modolcd = 0;
          mensaje(); // vuelve a pedir la contraseña
        }
      }

      if (key == 'D') { //Se desactiva la alarma
        modo = 2;//inicio
        escribir("DESACTIVADO", 0, 0);
      }
    }
    break;

    case 1://activacion

    if (key) {

      if (key == 'A') {
        modo = 4;//desarmar
        key = '\0';
        mensaje();
      }
    }
    if (digitalRead(infra) == HIGH) { //Deteccion de movimiento
      detectado = true;
    }


  if (detectado) { //Si hay movimiento suena el buzzer y avisa que se detecto algo
    tone(5, 500, 1000);
    Serial.println("detectado!");
  }
  
  break;

    case 2: // Desactivado

    if (key) {

      if (key == 'A') { //vuelve al modo armar
        modo = 0; //armar
        mensaje();
      }

      if (key == 'C') { // cambio de contraseña
        modo = 3;
        key = '\0';
        modolcd = 1;
        mensaje();
      }
    }
    break;

    case 3: //Cambiar contraseña

    if (key) {
      entrada += key;
      Serial.println(entrada);

      if (key != 'A' and key != 'B' and key != 'C' and key != 'D') {
        //lcd.print(key);
        lcd.print('*');
      }

      if (key == 'B') { //Borra la entrada
        lcd.clear();
        entrada = "";
      }

      if (key == 'A') { 
        entrada = entrada.substring(0, entrada.length() - 1);
        if (entrada == Contra) {//Verifica que la contraseña vieja sea igual a la ingresada
          modolcd = 2;
          mensaje(); // Pide nueva contraseña
          modo = 5; //repetir contraseña
          key = '\0';
          entrada = "";
        } else { // contraseña incorrecta avisa del error y vuelve a pedir contraseña
          escribir("Error!", 5, 0);
          entrada = "";
          tone(5, 100, 1000);
          delay(1000);
          modolcd = 1;
          mensaje();
        }
      }

      if (key == 'D') { //Desactiva la alarma
        modo = 2; //incio
        lcd.clear();
        lcd.print("DESACTIVADO");
      }


    }
    break;

    case 4:

    if (key) {
      entrada += key;
      Serial.println(entrada);

      if (key != 'A' and key != 'B' and key != 'C' and key != 'D') {
        //lcd.print(key);
        lcd.print('*');
      }

      if (key == 'B') {
        lcd.clear();
        entrada = "";
      }

      if (key == 'A') {
        entrada = entrada.substring(0, entrada.length() - 1);
        if (entrada == Contra) {
          detectado = false;
          escribir("DESACTIVADO", 0, 0);
          digitalWrite(led1, LOW);
          digitalWrite(led2, LOW);
          modo = 2; //inicio
          key = '\0';
          entrada = "";
          tone(5, 300, 500);
          delay(100);
          tone(5, 500, 500);

        } else {
          escribir("Error!", 5, 0);
          entrada = "";
          tone(5, 100, 1000);
          delay(1000);
          modolcd = 0;
          mensaje();
        }
      }

    }
    break;

    case 5: //Repetir nueva contraseña

    if (key) {
      entrada += key;
      Serial.println(entrada);

      if (key != 'A' and key != 'B' and key != 'C' and key != 'D') {
        //lcd.print(key);
        lcd.print('*');
      }

      if (key == 'B') {
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        entrada = "";
      }

      if (key == 'A') {
        entrada = entrada.substring(0, entrada.length() - 1);
        Contra = entrada; //Actualiza la contraseña
        escribir("MODIFICADO", 3, 0);
        tone(5, 300, 500);
        delay(100);
        tone(5, 500, 500);
        delay(2000);
        escribir("DESACTIVADO", 0, 0);
        modo = 2; //Inicio
      }

      if (key == 'D') {
        modo = 2; //Inicio
        escribir("DESACTIVADO", 0, 0);
      }
    }
    break;
  }
  }
  


/*void armar() {
  escribir("PASSWORD", 4, 0);
  lcd.setCursor(0, 1);
}

void desarmar() {
  escribir("PASSWORD", 4, 0);
  lcd.setCursor(0, 1);
}


void cambiarContra() {
  escribir("OLD PASSWORD", 2, 0);
  lcd.setCursor(0, 1);
}



void repetirContra() {
  escribir("NEW PASSWORD", 2, 0);
  lcd.setCursor(0, 1);
}*/
void mensaje(){

  switch(modolcd){

    case 0:

    escribir("PASSWORD", 4, 0);
    lcd.setCursor(0, 1);

    break;

    case 1:

    escribir("OLD PASSWORD", 2, 0);
    lcd.setCursor(0, 1);

    break;

    case 2:
    escribir("NEW PASSWORD", 2, 0);
    lcd.setCursor(0, 1);

    break;
  }
}

void activacion() { //Emite los tonos del buzzer y escribe en el lcd
  tone(5, 500, 1000);
  escribir("Activado en 1", 0, 0);
  delay(1500);
  tone(5, 500, 1000);
  escribir("Activado en 2", 0, 0);
  delay(1500);
  tone(5, 500, 1000);
  escribir("Activado en 3", 0, 0);
  delay(1500);
  tone(5, 500, 2000);
  delay(1000);
}



void escribir(String text, int col, int fila) {
  lcd.clear(); //Limpia la pantalla del lcd
  lcd.setCursor(col, fila);//posiciona el cursor
  lcd.print(text); //Muetra el texto
}


  



  
