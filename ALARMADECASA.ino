#include <LiquidCrystal_I2C.h> //libreria para el lcd
#include <Keypad.h> //libreria para el teclado
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);// declaro el lcd
//Defino los componentes que use
#define fotoR A1 
#define buzzer 5
#define infra 4
//Declaro las columnas y filas y sus teclas
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
 
 int ldr = 0; //variable para el valor de la fotoresistencia
 int modo= 0; //para cambiar en los distintos modos que hay
 String entrada; //almacena los datos
 String Contra = "6528";//contraseña default
 bool detectado = false; // para ver si entro alguien
//Las funciones armar desarmar cambiarcontraseña y repetir contraseña son para que el lcd muestre la palabra password
void armar();
void desarmar();
void cambiarContra();
void repetirContra();
void activacion(); //activacion es para la activacion de la alarma y para poner los distintos tonos del buzzer
void escribir(String text, int col, int fila);//para poder en una funcion mostrar la palabra con la declaracionde columnas y filas

void setup() {

  pinMode(fotoR, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(infra, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.print("DESACTIVADO");//Mensaje inicial señala que la alarma inicia desactivada
  
  Serial.begin(9600);

}

void loop() {

  char key = keypad.getKey();

  switch(modo) {//inician todos los modos de la alarma

    case 0: //Armar

    if (key) {
      entrada += key;

      if (key != 'A' and key != 'B' and key != 'C' and key != 'D') { // SON LAS TECLAS CON LOS MODOS
        lcd.print('*');
      }

      if (key == 'B') {
        lcd.clear();
        entrada = "";
      }

      if (key == 'A') {

        entrada = entrada.substring(0, entrada.length() - 1);

        if (entrada == Contra) {
          activacion();
          digitalWrite(4, HIGH);
          escribir("ACTIVADO", 0, 0);//escribir("ACTIVADO", 0, 0);
          modo = 1; //activacion
          key = '\0';
          entrada = "";
        } else {
          escribir("Error!", 5, 0);
          entrada = "";
          tone(5, 100, 1000);
          delay(1000);
          armar();
        }
      }

      if (key == 'D') {
        modo = 2;//inicio
        escribir("DESACTIVADO", 0, 0);
      }
    }
    break;

    case 1:

    if (key) {

      if (key == 'A') {
        modo = 4;//desarmar
        key = '\0';
        desarmar();
      }
    }

    ldr = analogRead(fotoR);

    if (ldr > 500) {
      detectado = true;
    }


  if (detectado) {
    tone(5, 500, 1000);
    Serial.println("detectado!");
  }
  
  break;

    case 2:

    if (key) {

      if (key == 'A') {
        modo = 0; //armar
        armar();
      }

      if (key == 'C') {
        modo = 3;//cambiar contraseña
        key = '\0';
        cambiarContra();
      }
    }
    break;

    case 3:

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
        if (entrada == Contra) {
          repetirContra();
          modo = 5; //repetir contraseña
          key = '\0';
          entrada = "";
        } else {
          escribir("Error!", 5, 0);
          entrada = "";
          tone(5, 100, 1000);
          delay(1000);
          cambiarContra();
        }
      }

      if (key == 'D') {
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
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(0, 1);
        entrada = "";
      }

      if (key == 'A') {
        entrada = entrada.substring(0, entrada.length() - 1);
        if (entrada == Contra) {
          detectado = false;
          escribir("DESACTIVADO", 0, 0);
          digitalWrite(4, LOW);
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
          desarmar();
        }
      }

    }
    break;

    case 5:

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
        Contra = entrada;
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
  


void armar() {
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
}


void activacion() {
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
  lcd.clear();
  lcd.setCursor(col, fila);
  lcd.print(text);
}


  
