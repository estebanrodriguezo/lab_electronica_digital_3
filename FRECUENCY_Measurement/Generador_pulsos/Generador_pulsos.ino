#include <TimerOne.h>
#include <Regexp.h>

/**
 * @brief Pin por donde saldrá la señal generada.
 */
int pinSignal = 9;
/**
 * @brief Concatena los caracteres recibidos y los concatena.
 */
String inputConsole;
/**
 * @brief Indica la variable de frecuencia incializada en 1kHz.
 */
long freq = 1000;  
/**
 * @brief Indica la variable de dureza inicializada en 50 que corresponde a 50%.
 */                                     
float duty = 50;
/**
 * @brief Se caulcula el periodo en microsegundos.
 */                                        
long cycleDuration = 1000000 / freq;     
/**
 * @brief Variable para calcular la dureza de ciclo según la resolución de 0 a 1023.
 */               
float dutyCylce = (duty / 100) * 1023; 
/**
 * @brief Bandera para indicar si se está en STOP o en START.
 */                 
bool stop = true;                                       
/**
 * @brief Variable para usar REGEX en los comandos de entrada.
 */
MatchState ms;                                          


/**
 * Esta función detecta cuando hay una entrada en consola
 * @brief Detecta cuando se ingresa un comando, concatena caracter por caracter y luego se valida con la función commands()
 * 
 */
void serialEvent() {
  while (Serial.available()) {                
    char charInput = Serial.read();
    inputConsole += charInput;
    if (charInput == '\n') {
      commands(inputConsole.substring(0, inputConsole.length() - 1)); 
      Serial.println(">>");
      inputConsole = "";
    }
  }
}


/**
 * Valida el comando ingresado por consola
 * @brief Recibe el comando ingresado y lo valida según las opciones, START, STOP, RESET, FREQ, DUTY, se usó REGEX para detectar correctamente cualquier comando y que fuera válido. Es donde se ejecunta todo el programa principalmente donde se cambia el valor de los métodos de la librería de Timer1.
 * 
 */
void commands(String command) {
  if (command == "START") {
    stop = false;
    Serial.println("SIG. ACTIVATED");
    
  } else if (command == "STOP") {
    stop = true;
    Serial.println("SIG. DISABLED");
    
  } else if (command.startsWith("FREQ")) {
    ms.Target(command.c_str());
    char result = ms.Match("^FREQ%s%d%d?%d?%d?%d?$", 0);

    if (result == REGEXP_MATCHED) {     
      // Coincide con el patron
      freq = inputConsole.substring(inputConsole.indexOf(" ") + 1).toInt();
      
      if (freq > 0 && freq <= 99999) {
        cycleDuration = 1000000 / freq;
        Serial.print("DONE ");
        Serial.println(1000000 / cycleDuration);
      } else {
        Serial.println("Comando inválido");
      }
    } else if (result == REGEXP_NOMATCH) {
      Serial.println("Comando inválido");
    }

  } else if (command.startsWith("DUTY")) {
    ms.Target(command.c_str());
    char result = ms.Match("^DUTY%s%d%d?$", 0);

    if (result == REGEXP_MATCHED) {
      
      duty = inputConsole.substring(inputConsole.indexOf(" ") + 1).toFloat();
      dutyCylce = (duty / 100) * 1023;
      
      if (duty > 0 && duty < 100) {
        Serial.println("DONE");
      } else {
        Serial.println("Comando inválido");
      }
    } else if (result == REGEXP_NOMATCH) {
      Serial.println("Comando inválido");
    }

  } else if (command == "RESET") {
      freq = 1000;
      duty = 50;
      cycleDuration = 1000000 / freq;
      dutyCylce = (duty / 100) * 1023;
  } else {
    Serial.println("Comando inválido");
  }

  Timer1.initialize(cycleDuration);
  Timer1.pwm(pinSignal, dutyCylce);
}

/**
 * Función para inicializar el programa
 * 
 */
void setup() {
  pinMode(pinSignal, OUTPUT);
  Serial.begin(9600);
  Serial.println(">>");
}

/**
 * Loop del programa
 * @brief Se ejecuta constamente comprobrando si la señal se encuentra en STOP o en START
 */
void loop() {
  if (stop) {
    digitalWrite(pinSignal, LOW);
  } 
}
