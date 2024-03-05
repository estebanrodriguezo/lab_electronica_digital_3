/**
 * @file FrecuenciaDisplay7Segmentos.ino
 * @brief Medidor de frecuencia con multiplexación en display de 7 segmentos
 * Este programa mide la frecuencia de una señal en una ventana de tiempo de 1 segundo y muestra el resultado
 * en un display de 6 dígitos de 7 segmentos.
 */

/**
 * @brief Cuenta los flancos de subida en una ventana de 1 segundo
 */
volatile int cuenta = 0;

/**
 * @brief Valor a mostrar en los displays
 */
unsigned int frequency = 0;

/**
 * @brief Variable para definir la ventana de 1 segundo
 */
unsigned long previous_time = 0;

/**
 * @brief Declaración de los segmentos de los displays
 */
int segmentPins[] = {7, 6, 18, 19, 20, 2, 1}; 

/**
 * @brief Pines digitales para controlar los displays.
 */
int disp1 = 10;
int disp2 = 11;
int disp3 = 12;
int disp4 = 13;
int disp5 = 14;
int disp6 = 15;

/**
 * @brief Arreglo hexadecimal de los números del 0 al 9
 */
int display7c[10] = {
  0b1000000, // 0
  0b1111001, // 1
  0b0100100, // 2
  0b0110000, // 3
  0b0011001, // 4
  0b0010010, // 5
  0b0000010, // 6
  0b1111000, // 7
  0b0000000, // 8
  0b0010000  // 9
};

/**
 * @brief Arreglo para controlar la multiplexación.
 */
int displays[6] = {disp1, disp2, disp3, disp4, disp5, disp6};

/**
 * @brief Indica si el último flanco detectado fue de subida o bajada
 */
volatile boolean flanco_subida = false;

/**
 * @brief Configuración de los pines y la interrupción
 */
void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }

  for (int i = 0; i < 6; i++) {
    pinMode(displays[i], OUTPUT); // Inicia pines de los displays
    digitalWrite(displays[i], HIGH); // Enciende todos los displays al inicio
  }

  pinMode(22, INPUT); // Pin que recibe la señal y donde se genera la interrupción

  // Indica qué función hacer al detectar la interrupción
  attachInterrupt(digitalPinToInterrupt(22), interrupcion, CHANGE);
  Serial.begin(9600);
}

/**
 * @brief Loop principal del programa en donde se mide la frecuencia y se controla la multiplexación.
 */
void loop() {
  // Ventana de tiempo 1 segundo
  if (millis() - previous_time > 1000) {
    frequency = cuenta;
    cuenta = 0;
    detachInterrupt(digitalPinToInterrupt(22)); // Desactivamos la interrupción mientras realizamos el cálculo
    flanco_subida = false; // Reseteamos el indicador de flanco de subida
    previous_time = millis();
    Serial.print("Frequency: ");
    Serial.print(frequency);
    Serial.println(" Hz");
    attachInterrupt(digitalPinToInterrupt(22), interrupcion, CHANGE); // Reactivamos la interrupción
  }

  update_display7(frequency);
}

/**
 * @brief Función de interrupción
 * @details Esta función se ejecuta cuando se hace una interrupción, cuenta los flancos de subida para el cálculo luego en una ventana de 1 segundo.
 */
void interrupcion() {
  if (flanco_subida) {
    cuenta++;
    flanco_subida = false;
  } else {
    flanco_subida = true;
  }
}

/**
 * @brief Actualización de los displays
 * @details Se ejecuta dicha función para actualizar el valor de los displays con los valores de frecuencia detectados.
 * @param number - Valor a mostrar en los displays.
 */
void update_display7(unsigned long number) {
  unsigned long number_temp = number;
  int digits = log10(number) + 1; // número de dígitos
  int cifra = 0;
  if (number_temp == 0) digits = 6; // Asegura que siempre se muestren 6 dígitos

  for (int i = 0; i < digits; i++) {
    digitalWrite(displays[i], LOW);
    
    if (number_temp <= 500000 && number_temp >= 1) {
      cifra = number % 10;
      number = number / 10;
    } else if (number_temp > 500000)
      cifra = 10;
    else if (number_temp == 0)
      cifra = 11;
    puerto(display7c[cifra]);
    delay(1);
    digitalWrite(displays[i], HIGH);
  }
}

/**
 * @brief Encender displays
 * @details Según sea la longitud de la frecuencia se encienden ciertos displays o no
 * @param bits - Bits que controlan los segmentos de los displays.
 */
void puerto(int bits) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(segmentPins[i], bitRead(bits, 6 - i));  // Invierte el orden de los bits para que coincida con el arreglo de pines
  }
}
