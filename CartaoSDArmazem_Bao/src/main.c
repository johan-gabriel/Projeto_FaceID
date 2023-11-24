#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include "storage.h"
#include "teste.h"




int main()
{
    stdio_init_all();
    sleep_ms(5000);
    if(!storageInit()){
        puts("Storage Faio!");
        //while (true) tight_loop_contents();
    }

    loop();

   //while (true) loop();
   
    return 0;
}

  