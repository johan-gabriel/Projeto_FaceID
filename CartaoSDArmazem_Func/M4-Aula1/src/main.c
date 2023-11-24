#include <stdio.h>
#include <pico/stdlib.h>

#include "storage.h"

int main()
{
    stdio_init_all();
	
	// Tenta inicializar o cartão SD e entra em loop infinito caso não consiga.
    if (!storageInit()) {
        puts("Storage falhou!");
        while (true) tight_loop_contents();
    }

	// Escreve uma linha de log no arquivo para teste.
    storageLog("123213213;123213213\n");

    while (true) tight_loop_contents();

    return 0;
}
