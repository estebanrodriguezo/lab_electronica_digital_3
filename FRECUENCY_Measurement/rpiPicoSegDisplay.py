import machine
import time

# Configura el pin GPIO 22 como entrada digital con resistencia de pull-down
pin = machine.Pin(22, machine.Pin.IN, machine.Pin.PULL_DOWN)

# Definición de pines GPIO para los segmentos (a, b, c, d, e, f, g) en el display de 7 segmentos
segment_pins = [1, 2, 20, 19, 18, 6, 7]

# Definición de pines GPIO para los dígitos (unidad, decena, centena, ...) en el display
digit_pins = [10, 11, 12, 13, 14, 15]

flancos = 0  # Variable para contar los flancos de la señal de entrada

# Nueva matriz de segmentos en formato binario
segments = [
    0b1000000,  # 0
    0b1111001,  # 1
    0b0100100,  # 2
    0b0110000,  # 3
    0b0011001,  # 4
    0b0010010,  # 5
    0b0000010,  # 6
    0b1111000,  # 7
    0b0000000,  # 8
    0b0010000   # 9
]

# Función para inicializar los pines de los segmentos y los dígitos
def initializePins():
    # Inicializar los pines de los segmentos como salidas y apagarlos
    for pin in segment_pins:
        machine.Pin(pin, machine.Pin.OUT).on()
    
    # Inicializar los pines de los dígitos como salidas y apagarlos
    for pin in digit_pins:
        machine.Pin(pin, machine.Pin.OUT).on()

# Función que se ejecutará cuando ocurra la interrupción en el pin 22
def contarFlancos(pin):
    global flancos
    flancos += 1

# Función para mostrar un dígito en el display de 7 segmentos
def displayDigit(digit):
    for i in range(7):
        pin = segment_pins[i]
        value = (segments[digit] >> i) & 1  # Obtener el valor del bit correspondiente
        machine.Pin(pin, machine.Pin.OUT).value(value)

# Función para mostrar un número en el display de 7 segmentos
def displayNumber(number):
    initializePins()
    digit = len(str(number)) - 1
    for i in str(number):
        displayDigit(int(i))
        machine.Pin(digit_pins[digit], machine.Pin.OUT).off()
        time.sleep(0.001)
        machine.Pin(digit_pins[digit], machine.Pin.OUT).on()
        digit -= 1

# Configura la interrupción en el pin 22 para contar flancos de la señal de entrada
pin.irq(trigger=machine.Pin.IRQ_RISING, handler=contarFlancos)

# Función principal del programa
def main():
    count = 0  
    frecuencia = 0
    global flancos
    # Registra el tiempo actual en microsegundos
    tiempo_inicio = time.ticks_us()
    while True:
        t = time.ticks_us()
        if(t - tiempo_inicio > 1000000):
            frecuencia = int(flancos)
            tiempo_inicio = time.ticks_us()
            flancos = 0
        displayNumber(frecuencia)

# Si este script es el programa principal que se ejecuta, se llama a la función initializePins()
# para configurar los pines y luego se inicia la función main()
if __name__ == "__main__":
    initializePins()  # Llama a la función de inicialización
    main()