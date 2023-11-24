#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pico/stdlib.h>

#include "ff.h"
#include "sd_card.h"
#include "storage.h"
#include "CH9121.h"
#include "teste.h"

int valConfig = 0;
int somer = 0;
void standby(){
    while (1)
    {
        CH9121_init();
        RX_TX();

        
    //      somer++;
    //  if(somer == 0){
    //         storageInit();
    //         storageLog("2;1;3\n");
    //         valConfig = readLog();
    //         printf("Valor da leitura: %d\n", valConfig);
            
    //     }
    //      if (somer == 2)
    //      { 
    //         storageInit();
    //          storageLog("0;0;1\n");
    //          valConfig = readLog();
    //          printf("Valor da leitura: %d\n", valConfig);
            
    //      }
    //      if (somer == 4)
    //      {
    //         storageInit();
    //          storageLog("0;1;0\n");
    //          valConfig = readLog();
    //          printf("Valor da leitura: %d\n", valConfig);
            
    //      }
    //     if (somer == 6)
    //     { 
    //         storageInit();
    //         storageLog("0;1;1\n");
    //         valConfig = readLog();
    //         printf("Valor da leitura: %d\n", valConfig);
            
    //     }
    //      if (somer == 8)
    //      { 
    //         storageInit();
    //          storageLog("1;0;0\n");
    //          valConfig = readLog();
    //          printf("Valor da leitura: %d\n", valConfig);
            
    //      }
    //      if (somer == 5)
    //      { 
    //         storageInit();
    //          storageLog("1;0;1\n");
    //          valConfig = readLog();
    //          printf("Valor da leitura: %d\n", valConfig);
            
    //      }
         
    }
     

}