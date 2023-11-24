#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>

#include "ff.h"
#include "sd_card.h"
#include "storage.h"
#include "teste.h"

int valConfig = 0;
int i = 0;
void loop(){
    while (1)
    {
         i++;
     if(i == 0){
            storageInit();
            storageLog("2;1;3\n");
            valConfig = readLog();
            printf("Valor da leitura: %d\n", valConfig);
            
        }
         if (i == 2)
         { 
            storageInit();
             storageLog("0;0;1\n");
             valConfig = readLog();
             printf("Valor da leitura: %d\n", valConfig);
            
         }
         if (i == 4)
         {
            storageInit();
             storageLog("0;1;0\n");
             valConfig = readLog();
             printf("Valor da leitura: %d\n", valConfig);
            
         }
        if (i == 6)
        { 
            storageInit();
            storageLog("0;1;1\n");
            valConfig = readLog();
            printf("Valor da leitura: %d\n", valConfig);
            
        }
         if (i == 8)
         { 
            storageInit();
             storageLog("1;0;0\n");
             valConfig = readLog();
             printf("Valor da leitura: %d\n", valConfig);
            
         }
         if (i == 5)
         { 
            storageInit();
             storageLog("1;0;1\n");
             valConfig = readLog();
             printf("Valor da leitura: %d\n", valConfig);
            
         }
         sleep_ms(500);
    }
     

}