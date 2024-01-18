// #include "pico/stdlib.h"

// #define BUZZER_PIN 6

// int main() {
//     // Inicializa o GPIO do buzzer como saída
//     gpio_init(BUZZER_PIN);
//     gpio_set_dir(BUZZER_PIN, GPIO_OUT);

//     while (1) {
//         // Liga o buzzer
//         gpio_put(BUZZER_PIN, 1);
//         sleep_ms(500);

//         // Desliga o buzzer
//         gpio_put(BUZZER_PIN, 0);
//         sleep_ms(500);
//     }
// }

#include "pico/stdlib.h"

#define BUZZER_PIN 6

// Função para tocar uma nota específica
void play_note(int note, int duration) {
    // Implemente esta função para tocar uma nota no buzzer
    // 'note' é a frequência da nota em Hz, e 'duration' é a duração em milissegundos
}

int main() {
    // Inicializa o GPIO do buzzer como saída
    gpio_init(BUZZER_PIN);
    gpio_set_dir(BUZZER_PIN, GPIO_OUT);

    // Define a melodia de "Jingle Bells"
    int melody[] = { 262, 262, 262, 262, 262, 262, 262, 294, 330, 262, 294, 330, 262 };
    int note_durations[] = { 500, 500, 1000, 500, 500, 1000, 500, 500, 500, 500, 500, 500, 1000 };
    int num_notes = sizeof(melody) / sizeof(int);

    while (1) {
        // Toca a melodia
        for (int i = 0; i < num_notes; i++) {
            play_note(melody[i], note_durations[i]);
            sleep_ms(note_durations[i]);
        }

        // Pausa por 1 segundo entre as repetições da melodia
        sleep_ms(1000);
    }
}