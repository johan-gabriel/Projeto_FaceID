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
int whichConfig = 0;
bool configurador = false;
void standby(){
    configurador = false;
    while (1)
    { 
        configurador = sinaliza();
        if (!configurador)

        {
            CH9121_init();
            RX_TX();
        }
        
       
         if (configurador)
         {
            //sleep_ms(5000);
            //  storageInit();
            //  whichConfig = sslConfig();
            //  printf("valor: %d", whichConfig);
            //  switch (whichConfig)
            //  {
            //     case 0:
            //         storageLog("0;0;0\n");
            //         valConfig = readLog();
            //         printf("Valor da leitura: %d\n", valConfig);
            //         break;
            //     case 1:
            //         storageLog("0;0;1\n");
            //         valConfig = readLog();
            //         printf("Valor da leitura: %d\n", valConfig);
            //         break;
                
            //     default:
            //         break;
             
             
            printf("Sinalizador OK!");
            configurador = false;
         }
         
    }
     

}