#include <pico/stdlib.h>
#include <stdio.h>

// Definindo linhas e colunas do teclado matricial
#define LINHAS_CONTAGEM 4
#define COL_COUNT 4
const uint8_t linhas[LINHAS_CONTAGEM] = {8, 7, 6, 5};
const uint8_t colunas[COL_COUNT] = {4, 3, 2, 28};

// Definições dos LEDs RGB
#define LED_RED 13
#define LED_BLUE 12
#define LED_GREEN 11

// Definição do buzzer
#define BUZZER 21

// Parâmetros de tons e durações
#define TONE_HIGH 2000
#define TONE_LOW 500
#define DURATION_SHORT 500
#define DURATION_LONG 1000

// Função auxiliar para configurar um GPIO como saída
void setup_output(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
}

// Função auxiliar para configurar um GPIO como entrada com pull-down
void setup_input(uint8_t pin) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_down(pin);
}

// Configuração inicial dos GPIOs
void setup_gpio() {
    // Configurando linhas do teclado como saídas
    for (int i = 0; i < LINHAS_CONTAGEM; i++) {
        setup_output(linhas[i]);
    }

    // Configurando colunas do teclado como entradas com pull-down
    for (int i = 0; i < COL_COUNT; i++) {
        setup_input(colunas[i]);
    }

    // Configurando LEDs RGB como saídas
    setup_output(LED_RED);
    setup_output(LED_BLUE);
    setup_output(LED_GREEN);

    // Configurando buzzer como saída
    setup_output(BUZZER);
}

// Função para acionar LEDs
void control_leds(uint8_t red, uint8_t blue, uint8_t green) {
    gpio_put(LED_RED, red);
    gpio_put(LED_BLUE, blue);
    gpio_put(LED_GREEN, green);
}

// Função para ativar e desativar o buzzer
void control_buzzer(uint8_t state) {
    gpio_put(BUZZER, state);
}

// Função para gerar tons no buzzer
void buzzer_tone(uint32_t frequency, uint32_t duration_ms) {
    uint32_t period_us = 1000000 / frequency; // Período em microssegundos
    uint32_t cycles = (frequency * duration_ms) / 1000; // Número de ciclos

    for (uint32_t i = 0; i < cycles; i++) {
        gpio_put(BUZZER, 1);
        sleep_us(period_us / 2);
        gpio_put(BUZZER, 0);
        sleep_us(period_us / 2);
    }
}

// Função para tocar um padrão de som no buzzer
void buzzer_pattern() {
    buzzer_tone(1000, 200);
    sleep_ms(100);
    buzzer_tone(1500, 200);
    sleep_ms(100);
    buzzer_tone(2000, 300);
}

// Função para ler o teclado matricial 4x4
char read_keypad() {
    const char keys[LINHAS_CONTAGEM][COL_COUNT] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    for (int r = 0; r < LINHAS_CONTAGEM; r++) {
        gpio_put(linhas[r], true); // Ativação da linha
        sleep_us(50);

        for (int c = 0; c < COL_COUNT; c++) {
            if (gpio_get(colunas[c])) { // Verificação de sinal na coluna
                gpio_put(linhas[r], false); // Desativa a linha
                return keys[r][c]; // Retorna a tecla pressionada
            }
        }
        gpio_put(linhas[r], false); // Desativa a linha
    }

    return 0;
}

int main() {
    stdio_init_all();
    setup_gpio();

    while (true) {
        char key = read_keypad(); // Leitura do teclado matricial 4x4

        if (key != 0) { // Se alguma tecla for pressionada
            printf("Tecla pressionada: %c\n", key);

            switch (key) {
                case 'A': // Liga o LED vermelho
                    control_leds(1, 0, 0);
                    break;
                case 'B': // Liga o LED azul
                    control_leds(0, 1, 0);
                    break;
                case 'C': // Liga o LED verde
                    control_leds(0, 0, 1);
                    break;
                case 'D': // Liga todos os LEDs
                    control_leds(1, 1, 1);
                    break;
                case '#': // Toca tom agudo no buzzer
                    buzzer_tone(TONE_HIGH, DURATION_SHORT);
                    break;
                case '*': // Toca tom grave no buzzer
                    buzzer_tone(TONE_LOW, DURATION_LONG);
                    break;
                case '0': // Toca um padrão de som no buzzer
                    buzzer_pattern();
                    break;
                default: // Desliga os LEDs
                    control_leds(0, 0, 0);
                    break;
            }
        }

        sleep_ms(100); // Delay para evitar leituras repetidas
    }

    return 0;
}