/*
  Codigo desarrollado por: Franco Joaquín Aguirre Elizalde
  Equipo: XpriT Robotics
  Contacto: Instagram @ag.franco.ok
  ----------------------------------------
  Descripción:
  Código optimizado para el control de sensores y visualización en pantalla OLED.
  Este programa es parte de los proyectos de XpriT Robotics,
  enfocado en la robótica competitiva y soluciones innovadoras.

  Fecha de creación: 2/12/2024
*/


#include <SPI.h>                 // Biblioteca para la comunicación SPI (Serial Peripheral Interface)
#include <Wire.h>                // Biblioteca para la comunicación I2C
#include <Adafruit_GFX.h>        // Biblioteca base para gráficos en pantallas
#include <Adafruit_SSD1306.h>    // Biblioteca específica para controlar pantallas OLED SSD1306

// Pines de control para los motores
#define MA1 11                    // Pin para controlar la dirección del Motor A
#define MA2 6                     // Otro pin para el Motor A
#define MB2 3                     // Pin para controlar la dirección del Motor B
#define MB1 5                     // Otro pin para el Motor B

// Pines para sensores de suelo
#define sueloD A1                // Sensor de suelo derecho
#define sueloI A7                // Sensor de suelo izquierdo

// Pines para sensores de oponente
#define LI 8                     // Sensor de oponente lateral izquierdo
#define FI 9                     // Sensor de oponente frontal izquierdo
#define FD A0                    // Sensor de oponente frontal derecho
#define LD 13                    // Sensor de oponente lateral derecho

// Pines para botones y control de inicio
#define GO 4                     // Botón para iniciar el programa
#define BTN2 2                   // Botón secundario (propósito no definido)
#define BTN1 7                   // Otro botón secundario (propósito no definido)

// Variables globales de configuración y estado
int strat = 0;                   // Variable para la estrategia del programa
int start = 0;                   // Variable para controlar el inicio
int maxVel = 0;                  // Variable para la velocidad máxima permitida
long int NOW = 0;
int waitStep = 4500;
int normalStep = 2000;

// Variables relacionadas con los sensores
int ssI, ssD, soI, sofI, sofD, soD = 0;  // Variables para almacenar lecturas de los sensores
int lMax = 1023;
int lMin = 700;

// Variables de control para el menú
int menuE = 0;                   // Estado del menú
int velM = 0;                    // Control de velocidad en el menú
int busMenu = 0;                 // Menú de configuración
int busMode = 0;                 // Modo de operación

// Configuración de la pantalla OLED
#define SCREEN_WIDTH 128         // Ancho de la pantalla OLED
#define SCREEN_HEIGHT 32         // Alto de la pantalla OLED
#define OLED_RESET -1            // Pin de reinicio de la pantalla (-1 si no se usa)
#define SCREEN_ADDRESS 0x3C      // Dirección I2C de la pantalla OLED

// Inicialización del objeto de la pantalla OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// para diseñar los iconos los pueden hacer en PAINT a una medida por ejemplo de 32x32 y luego los pasan al siguiente link
// https://javl.github.io/image2cpp/    y los copian como los siguientes arrays

const unsigned char logo [] PROGMEM = {             // Logo de equipo
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x70, 0x1c, 0xff, 0xf3, 0xff, 0xcf, 0x3f, 0xf0,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x00, 0xf8, 0x1e, 0xff, 0xf3, 0xff, 0xcf, 0x3f, 0xf0,
  0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0x00, 0x00, 0x78, 0x3c, 0xff, 0xf3, 0xff, 0xcf, 0x3f, 0xe0,
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x3c, 0x3c, 0x70, 0xf1, 0xc1, 0xcf, 0x07, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x04, 0x00, 0x3e, 0x78, 0x70, 0x73, 0xc1, 0xcf, 0x07, 0x00,
  0x08, 0x00, 0x00, 0x00, 0x7f, 0xc0, 0x1c, 0x80, 0x1f, 0xf0, 0xf0, 0xf3, 0xc1, 0xcf, 0x07, 0x00,
  0x04, 0x00, 0x00, 0x01, 0xff, 0x80, 0xf8, 0x80, 0x0f, 0xf0, 0xff, 0xf3, 0xc7, 0xcf, 0x07, 0x00,
  0x07, 0x00, 0x00, 0x07, 0xfe, 0x03, 0xe1, 0x00, 0x07, 0xe0, 0xff, 0xf3, 0xff, 0xcf, 0x07, 0x00,
  0x03, 0x80, 0x00, 0x0f, 0xf8, 0x0f, 0x83, 0x00, 0x07, 0xc0, 0xff, 0xf3, 0xff, 0x8f, 0x07, 0x00,
  0x01, 0xe0, 0x00, 0x3f, 0xe0, 0x3e, 0x03, 0x00, 0x07, 0xe0, 0xf7, 0xf3, 0xfe, 0x0f, 0x07, 0x00,
  0x00, 0xf0, 0x00, 0x7f, 0x80, 0xfe, 0x06, 0x00, 0x0f, 0xe0, 0xf0, 0x07, 0xff, 0x0f, 0x07, 0x00,
  0x01, 0x88, 0xf1, 0xff, 0x07, 0xff, 0x06, 0x00, 0x1f, 0xf0, 0xf0, 0x03, 0xcf, 0x0f, 0x07, 0x00,
  0x01, 0xe0, 0xf3, 0xfc, 0x1f, 0xff, 0x0c, 0x00, 0x3e, 0xf8, 0xf0, 0x03, 0xc7, 0xcf, 0x07, 0x80,
  0x00, 0x30, 0xff, 0xf0, 0x7f, 0xff, 0x8c, 0x00, 0xfc, 0x78, 0xf0, 0x03, 0xc7, 0xef, 0x07, 0x80,
  0x00, 0x1c, 0x7f, 0xc1, 0xff, 0xff, 0x9c, 0x00, 0xf8, 0x3c, 0xf0, 0x03, 0xc3, 0xef, 0x07, 0x80,
  0x00, 0x86, 0x3f, 0x07, 0xff, 0xff, 0xb8, 0x00, 0x70, 0x18, 0xe0, 0x03, 0xc1, 0xef, 0x03, 0x80,
  0x00, 0xc3, 0x0f, 0x3f, 0xff, 0xff, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x38, 0xc7, 0x3f, 0xff, 0xff, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x01, 0xe7, 0x07, 0xff, 0xff, 0xf0, 0x00, 0xfd, 0xfe, 0xfc, 0xff, 0xfe, 0xdf, 0xdf, 0xc0,
  0x00, 0x00, 0x13, 0x80, 0x7f, 0xff, 0xf0, 0x00, 0xff, 0xfe, 0xfc, 0xff, 0x7c, 0xdf, 0xcf, 0xc0,
  0x00, 0x01, 0x83, 0x8c, 0x0f, 0xff, 0xf0, 0x00, 0xc7, 0x86, 0x6c, 0xc3, 0x30, 0xd8, 0x0c, 0x00,
  0x00, 0x03, 0xf3, 0xcf, 0x81, 0xff, 0xf0, 0x00, 0xcd, 0x86, 0xfe, 0xc3, 0x30, 0xd8, 0x06, 0x00,
  0x00, 0x00, 0x01, 0xc3, 0xe0, 0x3f, 0xf0, 0x00, 0xfd, 0x86, 0xfe, 0xc3, 0x30, 0xd8, 0x03, 0x00,
  0x00, 0x00, 0x00, 0xe0, 0x00, 0x07, 0xf0, 0x01, 0xf9, 0x86, 0x66, 0xc3, 0x30, 0xd8, 0x01, 0x80,
  0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0xf0, 0x00, 0xdd, 0xc6, 0x66, 0xc3, 0x30, 0xdc, 0x49, 0xc0,
  0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0xe0, 0x00, 0xcf, 0xfe, 0xfe, 0xff, 0x38, 0xcf, 0xdf, 0xc0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0xfe, 0xde, 0xfe, 0x18, 0xc7, 0x8f, 0x80,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned char derecha [] PROGMEM = {      //icono estrategia 90° derecha
  0x00, 0x0f, 0xf0, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x80, 0x01, 0x00, 0x03, 0x00, 0x00, 0xc0,
  0x04, 0x00, 0x00, 0x20, 0x08, 0x00, 0x00, 0x10, 0x10, 0x00, 0x78, 0x08, 0x10, 0x00, 0x60, 0x08,
  0x20, 0x00, 0x50, 0x04, 0x40, 0x00, 0x48, 0x00, 0x40, 0x00, 0x04, 0x02, 0x40, 0x00, 0x02, 0x02,
  0x80, 0x00, 0x01, 0x01, 0x80, 0x00, 0x00, 0x81, 0x80, 0x00, 0x00, 0x41, 0x80, 0x00, 0x00, 0x21,
  0x80, 0x00, 0x00, 0x11, 0x80, 0x00, 0x00, 0x21, 0x80, 0x00, 0x00, 0x41, 0x80, 0x00, 0x00, 0x81,
  0x40, 0x00, 0x01, 0x02, 0x40, 0x00, 0x02, 0x02, 0x40, 0x00, 0x04, 0x02, 0x20, 0x00, 0x08, 0x04,
  0x10, 0x00, 0x10, 0x08, 0x10, 0x03, 0xe0, 0x08, 0x08, 0x02, 0xa0, 0x10, 0x04, 0x03, 0xe0, 0x20,
  0x03, 0x03, 0xe0, 0xc0, 0x00, 0x83, 0xe1, 0x00, 0x00, 0x30, 0x0e, 0x00, 0x00, 0x0f, 0xf0, 0x00
};

const unsigned char izquierda [] PROGMEM = {      //icono estrategia 90° izquierda
  0x00, 0x0f, 0xf0, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x80, 0x01, 0x00, 0x03, 0x00, 0x00, 0xc0,
  0x04, 0x00, 0x00, 0x20, 0x08, 0x00, 0x00, 0x10, 0x10, 0x1e, 0x00, 0x08, 0x10, 0x06, 0x00, 0x08,
  0x20, 0x0a, 0x00, 0x04, 0x00, 0x12, 0x00, 0x02, 0x40, 0x20, 0x00, 0x02, 0x40, 0x40, 0x00, 0x02,
  0x80, 0x80, 0x00, 0x01, 0x81, 0x00, 0x00, 0x01, 0x82, 0x00, 0x00, 0x01, 0x84, 0x00, 0x00, 0x01,
  0x88, 0x00, 0x00, 0x01, 0x84, 0x00, 0x00, 0x01, 0x82, 0x00, 0x00, 0x01, 0x81, 0x00, 0x00, 0x01,
  0x40, 0x80, 0x00, 0x02, 0x40, 0x40, 0x00, 0x02, 0x40, 0x20, 0x00, 0x02, 0x20, 0x10, 0x00, 0x04,
  0x10, 0x08, 0x00, 0x08, 0x10, 0x07, 0xc0, 0x08, 0x08, 0x05, 0x40, 0x10, 0x04, 0x07, 0xc0, 0x20,
  0x03, 0x07, 0xc0, 0xc0, 0x00, 0x87, 0xc1, 0x00, 0x00, 0x70, 0x0c, 0x00, 0x00, 0x0f, 0xf0, 0x00
};


const unsigned char redondo [] PROGMEM = {      //icono estrategia giro por el tatami
  0x00, 0x0f, 0xf0, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x80, 0x01, 0x00, 0x03, 0x00, 0x00, 0xc0,
  0x04, 0x00, 0x00, 0x20, 0x08, 0x07, 0xe0, 0x10, 0x10, 0x18, 0x18, 0x08, 0x10, 0x60, 0x06, 0x08,
  0x20, 0x80, 0x01, 0x04, 0x41, 0x00, 0x01, 0x00, 0x41, 0x00, 0x00, 0x82, 0x42, 0x00, 0x00, 0x42,
  0x82, 0x00, 0x00, 0x41, 0x84, 0x00, 0x00, 0x21, 0x84, 0x00, 0x00, 0x21, 0xa4, 0x80, 0x00, 0x21,
  0x95, 0x00, 0x00, 0x21, 0x8e, 0x00, 0x00, 0x21, 0x84, 0x00, 0x00, 0x21, 0x80, 0x00, 0x00, 0x41,
  0x40, 0x00, 0x00, 0x42, 0x40, 0x00, 0x00, 0x82, 0x40, 0x00, 0x00, 0x82, 0x20, 0x00, 0x01, 0x04,
  0x10, 0x00, 0x06, 0x08, 0x10, 0x03, 0xf8, 0x08, 0x08, 0x02, 0xa0, 0x10, 0x04, 0x03, 0xe0, 0x20,
  0x03, 0x03, 0xe0, 0xc0, 0x00, 0x83, 0xe1, 0x00, 0x00, 0x30, 0x0e, 0x00, 0x00, 0x0f, 0xf0, 0x00
};

void calibrarLinea() {

  display.clearDisplay();
  display.setCursor(30, 12);
  display.write("Calibrando Linea");
  display.display();
  lMin = 1023;
  lMax = 0;
  if (analogRead(sueloD) < lMin) lMin = analogRead(sueloD);
  if (analogRead(sueloI) > lMax) lMax = analogRead(sueloI);
  if (analogRead(sueloI) < lMin) lMin = analogRead(sueloI);
  if (analogRead(sueloD) > lMax) lMax = analogRead(sueloD);
  delay(1000);
}

void DisplayLine() {
  display.clearDisplay();
  display.setCursor(0, 5);
  display.write("Line Max: ");
  display.setCursor(60, 5);
  display.print(lMax);
  display.setCursor(0, 19);
  display.write("Line Min: ");
  display.setCursor(60, 19);
  display.print(lMin);
  display.display();
  delay(1000);
}


// Función para dibujar una barra analógica en la pantalla OLED.
// Representa gráficamente un valor analógico en una barra vertical.
void drawAnalogBar(int sensorValue, int x, int width) {
  int barHeight = map(sensorValue, 0, 1023, 0, SCREEN_HEIGHT);
  // Mapea el valor del sensor (0-1023) a la altura de la pantalla OLED.

  // Dibuja una barra vertical proporcional al valor analógico del sensor.
  display.fillRect(x, SCREEN_HEIGHT - barHeight, width, barHeight, SSD1306_WHITE);
}

// Función para dibujar una barra digital en la pantalla OLED.
// Representa gráficamente un valor digital (0 o 1) en una barra vertical.
void drawDigitalBar(int sensorValue, int x, int width) {
  int barHeight = map(sensorValue, 0, 1, 0, SCREEN_HEIGHT);
  // Mapea el valor digital (0-1) a la altura de la pantalla.

  // Dibuja una barra vertical proporcional al valor digital del sensor.
  display.fillRect(x, SCREEN_HEIGHT - barHeight, width, barHeight, SSD1306_WHITE);
}


// Función para controlar y mostrar un menú de selección de estrategias y configuraciones.
void stratSelect() {
  // Incrementa el índice del menú al presionar BTN2.
  if (digitalRead(BTN2)) {
    menuE++; // Cambia a la siguiente opción del menú.
    delay(300); // Retardo para evitar múltiples incrementos no intencionados.
  }

  display.setTextSize(1.1); // Ajusta el tamaño del texto.
  display.setTextColor(SSD1306_WHITE); // Configura el color del texto.

  int maxStrat = 2;  // Número máximo de estrategias disponibles.
  int maxVelM = 8;   // Número máximo de niveles de velocidad.
  int maxBusMenu = 2; // Número máximo de modos de búsqueda.

  // Gestión de la selección de estrategias.
  if (menuE == 0) {
    if (digitalRead(BTN1)) {
      strat++; // Cambia a la siguiente estrategia.
      delay(300); // Retardo para evitar múltiples cambios no intencionados.
    }
  }

  // Muestra un ícono en función de la estrategia seleccionada.
  if (strat == 0) {
    display.clearDisplay();
    display.drawBitmap(0, 0, derecha, 32, 32, SSD1306_WHITE); // Estrategia derecha.
  }
  if (strat == 1) {
    display.clearDisplay();
    display.drawBitmap(0, 0, izquierda, 32, 32, SSD1306_WHITE); // Estrategia izquierda.
  }
  if (strat == 2) {
    display.clearDisplay();
    display.drawBitmap(0, 0, redondo, 32, 32, SSD1306_WHITE); // Estrategia redonda.
  }

  // Reinicia la estrategia si excede el límite.
  if (strat > maxStrat) {
    strat = 0;
  }

  // Gestión de la configuración de velocidad máxima.
  if (menuE == 1) {
    if (digitalRead(BTN1)) {
      velM++; // Cambia al siguiente nivel de velocidad.
      delay(300);
    }
  }

  // Configura la velocidad máxima según la selección.
  if (velM == 0) {
    display.setCursor(40, 0);
    display.write("max vel: 20%");
    maxVel = map(20,0,100,0,255); // 20% de la velocidad máxima.
  }
  if (velM == 1) {
    display.setCursor(40, 0);
    display.write("max vel: 30%");
    maxVel = map(30,0,100,0,255); // 30% de la velocidad máxima.
  }
  if (velM == 2) {
    display.setCursor(40, 0);
    display.write("max vel: 40%");
    maxVel = map(40,0,100,0,255); // 40% de la velocidad máxima.
  }
  if (velM == 3) {
    display.setCursor(40, 0);
    display.write("max vel: 50%");
    maxVel = map(50,0,100,0,255); // 50% de la velocidad máxima.
  }
  if (velM == 4) {
    display.setCursor(40, 0);
    display.write("max vel: 60%");
    maxVel = map(60,0,100,0,255); // 60% de la velocidad máxima.
  }
  if (velM == 5) {
    display.setCursor(40, 0);
    display.write("max vel: 70%");
    maxVel = map(70,0,100,0,255); // 70% de la velocidad máxima.
  }
  if (velM == 6) {
    display.setCursor(40, 0);
    display.write("max vel: 80%");
    maxVel = map(80,0,100,0,255); // 80% de la velocidad máxima.
  }
  if (velM == 7) {
    display.setCursor(40, 0);
    display.write("max vel: 90%");
    maxVel = map(90,0,100,0,255); // 90% de la velocidad máxima.
  }
  if (velM == 8) {
    display.setCursor(40, 0);
    display.write("max vel: 100%");
    maxVel = map(100,0,100,0,255); // 100% de la velocidad máxima.
  }

  // Reinicia la velocidad si excede el límite.
  if (velM > maxVelM) {
    velM = 0;
  }

  // Gestión de los modos de búsqueda.
  if (menuE == 2) {
    if (digitalRead(BTN1)) {
      busMenu++; // Cambia al siguiente modo de búsqueda.
      delay(300);
    }
  }

  // Reinicia el modo de búsqueda si excede el límite.
  if (busMenu > maxBusMenu) {
    busMenu = 0;
  }

  // Configura el modo de búsqueda según la selección.
  if (busMenu == 0) {
    display.setCursor(40, 12);
    display.write("Search: Slow");
    busMode = 0; // Modo de búsqueda lento.
  }
  if (busMenu == 1) {
    display.setCursor(40, 12);
    display.write("Search: waitSteps");
    busMode = 1; // Modo de búsqueda rápido.
  }
  if (busMenu == 2) {
    display.setCursor(40, 12);
    display.write("Search: steps");
    busMode = 2; // Modo de búsqueda por pasos.
  }

  // Indica que el robot está listo para comenzar.
  if (menuE == 3) {
    display.setCursor(40, 24);
    display.write("Ready");
    display.display();

    // Espera hasta que se presione el botón de inicio (GO).
    while (!digitalRead(GO)) {}

    // Muestra el mensaje de inicio.
    display.setCursor(90, 24);
    display.write("GO!!!");
    start = 1; // Marca el estado de inicio.
  }

  // Actualiza la pantalla con los cambios realizados.
  display.display();
}

void motores(int mi, int md) {
  if (mi > 0) {
    analogWrite(MA1, abs(mi));
    analogWrite(MA2, 0);
  } else {
    analogWrite(MA1, 0);
    analogWrite(MA2, abs(mi));
  }
  if (md > 0) {
    analogWrite(MB1, abs(md));
    analogWrite(MB2, 0);
  } else {
    analogWrite(MB1, 0);
    analogWrite(MB2, abs(md));
  }
}



int online() {
  if ((ssI > lMin + 40) && (ssD > lMin + 40)) {
    return 1;
  } else if ((ssI > lMin + 40) && (ssD <= lMin + 40)) {
    motores(-100, -100);
    delay(50);
    motores(-100, 80);
    delay(50);
    return 0;
  } else if ((ssI <= lMin + 40) && (ssD > lMin + 40)) {
    motores(-100, -100);
    delay(50);
    motores(80, -100);
    delay(50);
    return 0;
  } else if ((ssI <= lMin + 40) && (ssD <= lMin + 40)) {
    motores(-100, -100);
    delay(100);
    motores(-100, 80);
    delay(50);
    return 0;
  }
}

void busqueda() {
  if (busMode == 0) {
    motores(42, 34);
  }
  if (busMode == 1) {
    if (millis() > NOW) {
      motores(50, 50);
      delay(10);
      NOW = millis() + waitStep;
    }
    else {
      motores(0, 0);
    }
  }
  if (busMode == 2) {
    if (millis() > NOW) {
      motores(50, 50);
      delay(30);
      NOW = millis() + normalStep;
    }
    else {  
      motores(0,0);
    }
  }
}

void stratStart(){
  if (strat == 2){
    motores(100,100);
    delay(70);
    motores(50,235);
    delay(500);
    motores(-60,130);
    delay(100);
  }
}


void setup() {
  // Configuración inicial del sistema.
  // Inicializa los pines, la pantalla OLED, y muestra el logo al inicio.
  pinMode(MA1, OUTPUT); // Configura MA1 como salida para el motor A.
  pinMode(MA2, OUTPUT); // Configura MA2 como salida para el motor A.
  pinMode(MB1, OUTPUT); // Configura MB1 como salida para el motor B.
  pinMode(MB2, OUTPUT); // Configura MB2 como salida para el motor B.
  pinMode(GO, INPUT);   // Configura el botón de inicio como entrada.
  pinMode(BTN1, INPUT); // Configura el botón 1 del menú como entrada.
  pinMode(BTN2, INPUT); // Configura el botón 2 del menú como entrada.

  display.setTextSize(1.1);
  display.setTextColor(SSD1306_WHITE);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.clearDisplay(); // Limpia la pantalla OLED.
  display.drawBitmap(0, 0, logo, 128, 32, SSD1306_WHITE); // Muestra el logo almacenado en la memoria.
  display.display();  // Actualiza la pantalla para mostrar el logo.
  delay(2000); // Pausa de 2 segundos para que el logo sea visible.
  if (digitalRead(BTN2)) {
    calibrarLinea();
    delay(1000);

  }
  DisplayLine();
  while (!start) {
    stratSelect();
  }
  delay(4890);
  stratStart();
}

void loop() {
  // Limpia la pantalla OLED antes de dibujar nuevos elementos.
  display.clearDisplay();

  // Lee el valor analógico del sensor de suelo izquierdo (sueloI) y lo almacena en ssI.
  ssI = analogRead(sueloI);

  // Lee los valores de los sensores de oponentes y los almacena en sus variables
  soI = digitalRead(LI);    // Estado del sensor lateral izquierdo.
  sofI = digitalRead(FI);   // Estado del sensor frontal izquierdo.
  sofD = digitalRead(FD);   // Estado del sensor frontal derecho.
  soD = digitalRead(LD);    // Estado del sensor lateral derecho.

  // Lee el valor analógico del sensor de suelo derecho (sueloD) y lo almacena en ssD.
  ssD = analogRead(sueloD);

  // Dibuja una barra analógica en la pantalla OLED para representar el valor de ssI.
  drawAnalogBar(ssI, 1, 20);
  // `x = 1` es la posición horizontal de la barra, `width = 20` es el ancho.

  // Dibuja barras digitales en la pantalla OLED para representar los estados binarios de los sensores.
  drawDigitalBar(soI, 22, 20);   // Barra para el estado de soI.
  drawDigitalBar(sofI, 43, 20);  // Barra para el estado de sofI.
  drawDigitalBar(sofD, 64, 20);  // Barra para el estado de sofD.
  drawDigitalBar(soD, 85, 20);   // Barra para el estado de soD.

  // Dibuja una barra analógica en la pantalla OLED para representar el valor de ssD.
  drawAnalogBar(ssD, 106, 20);
  // `x = 106` es la posición horizontal de la barra, `width = 20` es el ancho.

  // Actualiza la pantalla OLED para mostrar los elementos dibujados.

  if (digitalRead(GO)) {
    if (online()) {
      if ((soI)&&(!sofI)&&(!sofD)&&(!soD))motores(-120,120);
      if ((!soI)&&(sofI)&&(!sofD)&&(!soD))motores(-60,60);
      if ((!soI)&&(sofI)&&(sofD)&&(!soD))motores(maxVel,maxVel);
      if ((!soI)&&(!sofI)&&(sofD)&&(!soD))motores(60,-60);
      if ((!soI)&&(!sofI)&&(!sofD)&&(soD))motores(120,-120);
      if ((!soI)&&(!sofI)&&(!sofD)&&(!soD))busqueda();
    }
  } else {
    motores(0, 0);
  }

  display.display();
  // Espera 5 milisegundos antes de continuar con la siguiente iteración o bloque de código.
  delay(5);
}
