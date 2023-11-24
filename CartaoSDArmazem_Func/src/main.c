#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include "storage.h"
#include "CH9121.h"
#include "teste.h"

int valConfigMain = 0;

int main()
{
    stdio_init_all();
    sleep_ms(5000);
    if(!storageInit()){
        puts("Storage Faio!");
        //while (true) tight_loop_contents();
    }
    valConfigMain = readLog();
    printf("Valor da leitura: %d\n", valConfigMain);
    standby();

   //while (true) loop();
   
    return 0;
}

  