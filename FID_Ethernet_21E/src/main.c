#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>
#include "storage.h"
#include "CH9121.h"
#include "SP.h"

int valConfigMain = 0;

int main()
{
    stdio_init_all();
    sleep_ms(5000);
    if(!storageInit()){
        puts("Storage Faio!");
    }
    
    standby();

   
   
    return 0;
}

  