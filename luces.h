//Se crean macros que asignan nombres a los números de los pines GPIO asociados con tres LEDs diferentes. 
#define LED_AMARILLO 10 // GPIO 0 para el LED amarillo
#define LED_VERDE 1     // GPIO 1 para el LED verde
#define LED_ROJO 11     // GPIO 11 para el LED rojo

// Configurar pines GPIO para LEDs como salidas

//Funcion para encendere el led amarillo
void encenderledamarillo()
{
    gpio_init(LED_AMARILLO); //Inicializa el pin GPIO asociado con el LED amarillo utilizando gpio_init().
    gpio_set_dir(LED_AMARILLO, GPIO_OUT); //Se establece la dirección del pin como salida con gpio_set_dir()
    gpio_put(LED_AMARILLO, 1); //Se pone el valor del pin en 1 (gpio_put(LED_AMARILLO, 1)) para encender el LED.
}
//Funcion para apagar el led amarillo
void apagarledamarillo()
{
    gpio_init(LED_AMARILLO);//Inicializa el pin GPIO asociado con el LED amarillo utilizando gpio_init().
    gpio_set_dir(LED_AMARILLO, GPIO_OUT); //Se establece la dirección del pin como salida con gpio_set_dir()
    gpio_put(LED_AMARILLO, 0); //Se pone el valor del pin en 0 (gpio_put(LED_AMARILLO, 0)) para apagar el LED.
}
//Funcion para hacer parpadear el led amarillo
void parpadearledamarillo()
{
    // Establecer funcion para parpadear a 1Hz
    gpio_init(LED_AMARILLO);//Inicializa el pin GPIO asociado con el LED amarillo utilizando gpio_init().
    gpio_set_dir(LED_AMARILLO, GPIO_OUT);//Se establece la dirección del pin como salida con gpio_set_dir()
    gpio_put(LED_AMARILLO, 1);//Se pone el valor del pin en 1 (gpio_put(LED_AMARILLO, 1)) para encender el LED.
    sleep_ms(500); // Definir esto para que parpadee a 1HZ
    gpio_put(LED_AMARILLO, 0);//Se pone el valor del pin en 0 (gpio_put(LED_AMARILLO, 0)) para APAGAR el LED.
    sleep_ms(500);
}

void encenderledverde()
{

    gpio_init(LED_VERDE);//Inicializa el pin GPIO asociado con el LED verde utilizando gpio_init().
    gpio_set_dir(LED_VERDE, GPIO_OUT);//Se establece la dirección del pin como salida con gpio_set_dir()
    gpio_put(LED_VERDE, 1);//Se pone el valor del pin en 1 (gpio_put(LED_VERDE, 1)) para encender el LED.
    sleep_ms(10000); //Se inicia un timer que apagara el led transcurridos 10s
    gpio_put(LED_VERDE, 0);//Se pone el valor del pin en 0 (gpio_put(LED_VERDE, 0)) para APAGAR el LED.
}

void apagarledverde()
{
    gpio_init(LED_VERDE);//Inicializa el pin GPIO asociado con el LED verde utilizando gpio_init().
    gpio_set_dir(LED_VERDE, GPIO_OUT);//Se establece la dirección del pin como salida con gpio_set_dir()
    gpio_put(LED_VERDE, 0);//Se pone el valor del pin en 0 (gpio_put(LED_VERDE, 0)) para APAGAR el LED.
}

void encenderledrojo()
{
    gpio_init(LED_ROJO);//Inicializa el pin GPIO asociado con el LED rojo utilizando gpio_init().
    gpio_set_dir(LED_ROJO, GPIO_OUT);//Se establece la dirección del pin como salida con gpio_set_dir()
    gpio_put(LED_ROJO, 1);//Se pone el valor del pin en 1 (gpio_put(LED_ROJO, 1)) para encender el LED.
    sleep_ms(3000); //Se inicia un timer que apagara el led transcurridos 3s
    gpio_put(LED_ROJO, 0);//Se pone el valor del pin en 0 (gpio_put(LED_ROJO, 0)) para APAGAR el LED.
}

void apagarledrojo()
{
    gpio_init(LED_ROJO);//Inicializa el pin GPIO asociado con el LED rojo utilizando gpio_init().
    gpio_set_dir(LED_ROJO, GPIO_OUT);//Se establece la dirección del pin como salida con gpio_set_dir()
    gpio_put(LED_ROJO, 0);//Se pone el valor del pin en 0 (gpio_put(LED_ROJO, 0)) para APAGAR el LED.
}